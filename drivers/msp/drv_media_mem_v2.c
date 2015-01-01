/* media-mem.c
*
* Copyright (c) 2006 Hisilicon Co., Ltd. 
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
*/

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36))
#include <linux/smp_lock.h>
#endif
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <mach/hardware.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/spinlock.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/list.h>
#include <asm/cacheflush.h>
#include <linux/time.h>
#include <asm/setup.h>

#include "drv_media_mem.h"
#include "hi_kernel_adapt.h"

#define DEFAULT_ALLOC   0
#define SLAB_ALLOC          1
#define EQ_BLOCK_ALLOC  2
#if  MMB_ALLOC_OPTIMIZE
#define ADVANCE_ALLOC    4
#endif

#define LOW_TO_HIGH    0
#define HIGH_TO_LOW    1

#define MMZ_DBG_LEVEL 0x0
#if  MMZ_DBG_LEVEL
#define mmz_trace(level, s, params...) do{ if (level & MMZ_DBG_LEVEL)\
        printk(KERN_INFO "[%s, %d]: " s "\n", __FUNCTION__, __LINE__, params);\
        }while(0)

#define mmz_trace_func() mmz_trace(0x02,"%s", __FILE__)
#else
#define mmz_trace(level, s, params...)      
#define mmz_trace_func()                    
#endif

#define MMZ_GRAIN PAGE_SIZE
#define mmz_bitmap_size(p) (mmz_align2(mmz_length2grain((p)->nbytes),8)/8)

#define mmz_get_bit(p,n) (((p)->bitmap[(n)/8]>>((n)&0x7))&0x1)
#define mmz_set_bit(p,n) (p)->bitmap[(n)/8] |= 1<<((n)&0x7)
#define mmz_clr_bit(p,n) (p)->bitmap[(n)/8] &= ~(1<<((n)&0x7))

#define mmz_pos2phy_addr(p,n) ((p)->phys_start+(n)*MMZ_GRAIN) 
#define mmz_phy_addr2pos(p,a) (((a)-(p)->phys_start)/MMZ_GRAIN) 

#define mmz_align2low(x,g) (((x)/(g))*(g))
#define mmz_align2(x,g) ((((x)+(g)-1)/(g))*(g))
#define mmz_grain_align(x) mmz_align2(x,MMZ_GRAIN)
#define mmz_length2grain(len) (mmz_grain_align(len)/MMZ_GRAIN)

#define begin_list_for_each_mmz(p,gfp,mmz_name) list_for_each_entry(p,&mmz_list, list) {\
        if (gfp==0 ? 0:(p)->gfp!=(gfp))\
            continue;\
        if (mmz_name!=NULL) { \
            if ((*mmz_name != '\0') && strcmp(mmz_name, p->name)) \
                continue; \
        } \
        if ((mmz_name==NULL) && (anony==1)) { \
            if (strcmp("anonymous", p->name)) \
                continue; \
        }

#define end_list_for_each_mmz() }

char line[COMMAND_LINE_SIZE];
int mmz_total_size = 0;
int mmz_used_size = 0;
int mmz_free_size = 0;
int zone_number = 0;
static unsigned long _strtoul_ex(const char *s, char **ep, unsigned int base)
{
    char *__end_p;
    unsigned long __value;
    
    __value = simple_strtoul(s,&__end_p,base); 

    switch (*__end_p) { 
    case 'm': 
    case 'M':
        __value <<= 10; 
    case 'k': 
    case 'K': 
        __value <<= 10; 
        if (ep)
            (*ep) = __end_p + 1;
    default: 
        break; 
    } 

    return __value;
}

static LIST_HEAD(mmz_list);
static HI_DECLARE_MUTEX(mmz_lock);

static int anony = 0;
static int mmz_info_phys_start = -1;

module_param(anony, int, S_IRUGO);

#if  MMB_ALLOC_OPTIMIZE
#define MMZ_OPTIMIZE_DEBUG    0
#define AREA_PER_ALL                128
#ifdef MMB_LIST_OPTIMIZE
#define MMB_NUMS_INFLEXTION  12//20//0
#endif

struct mmb_pages_area{
    unsigned int page_min;
    unsigned int page_max;
    unsigned int percent;
};

static const struct mmb_pages_area s_alloc_pages_area[ALLOC_PAGES_MAX_NUM] = {
//page_min   page_max  percent/AREA_PER_ALL
        /*
        {1,           1,              2},
        {2,           2,              2},
        {3,           3,              1},
        {4,           4,              2},
        {5,           5,              1},
        {6,           6,              1},
        {7,           7,              1},
        {8,           8,              2},
        {9,           0xffff,        116},   */  
        /*
        {1,           1,              2},
        {2,           2,              2},
        {3,           5,              2},
        {6,           6,              2},
        {7,           7,              2},
        {8,           9,              2},
        {10,         12,             2},
        {13,         16,             2},
        {17,         0xffff,         112}, */  
        {1,           1,              2},
        {2,           2,              2},
        {3,           3,              3},
        {4,           4,              9},
        {5,           7,              4},
        {8,           9,              2},
        {10,         12,             2},
        {13,         16,             2},
        {17,         0xffff,       102},   
};

static unsigned int get_page_area_index( unsigned int page)
{
    unsigned int i = 0;
    for (; i < ALLOC_PAGES_MAX_NUM; i++) {
        if (s_alloc_pages_area[i].page_min <= page && s_alloc_pages_area[i].page_max >= page)  break;
    }

    return i;
}

static int check_page_area_percent(void)
{
    int i = 0;
    int all = 0;
    for (; i < ALLOC_PAGES_MAX_NUM; i++) {
        all += s_alloc_pages_area[i].percent;
    }

    if (all <= AREA_PER_ALL) return 0;
    return 1;
}

#if  MMZ_OPTIMIZE_DEBUG
static int mmz_list_check(hil_mmz_t *zone, int free_list, int mmb_list)
{
    struct mmb_free_node *p_free_nodes = NULL;
    hil_mmb_t *p;
    unsigned long temp_addr = 0;
    int ret = 0;

     if  (free_list) {
       list_for_each_entry(p_free_nodes, &zone->free_list, list) {
              if(p_free_nodes->addr_start <= temp_addr) {
                   ret = 1;
                   break;
              }
              temp_addr = p_free_nodes->addr_start;
          }
      }

      temp_addr = 0;
      if  (mmb_list  && ret == 0) {
          list_for_each_entry(p,&zone->mmb_list, list) {
              if(p->phys_addr <= temp_addr) {
                   ret = 1;
                   break;
              }
              temp_addr = p->phys_addr;
          }  
      }
      
      return ret;
}

static void mmz_optimize_debug(hil_mmz_t *zone, int free_list, int mmb_list, int start_node)
{
    int i;
    struct mmb_free_node *p_free_nodes = NULL;
    hil_mmb_t *p;
    int num = 0;

    //for (i = 0; i < ALLOC_PAGES_MAX_NUM; i++) {
    //    printk(KERN_INFO "%d : [%lx, %lx]\n",(i+1), zone->alloc_start_addr[i],  (i == ALLOC_PAGES_MAX_NUM - 1)?(zone->phys_start + zone->nbytes): (zone->alloc_start_addr[i+1] -1));
    //}

    if  (start_node) {
        for (i = 0; i < ALLOC_PAGES_MAX_NUM; i++) {
            p_free_nodes = list_entry(zone->alloc_start_node[i], struct mmb_free_node, list);
            printk(KERN_INFO "list-point.%d:[%lx, %lx]\n",i,p_free_nodes->addr_start,p_free_nodes->mmb_size);
        }  
    }  
    
    if  (free_list) {
        list_for_each_entry(p_free_nodes, &zone->free_list, list) {
           for (i = 0; i < (ALLOC_PAGES_MAX_NUM - 1); i++) {
                if  (p_free_nodes->addr_start >= zone->alloc_start_addr[i] && p_free_nodes->addr_start < zone->alloc_start_addr[i+1]) {
                       break;
                }               
            }         
            printk(KERN_INFO "VALID:%d. [%lx, %lx]\n",i, p_free_nodes->addr_start,p_free_nodes->mmb_size);
        }
    }
   
    if  (mmb_list) {
        list_for_each_entry(p,&zone->mmb_list, list) {
            num++;
            printk(KERN_INFO "MMB :[%lx, %lx].%d.%s\n",p->phys_addr,p->length,num,p->name);
        }  
    }
      
#if MMB_LIST_OPTIMIZE
    printk(KERN_INFO " ----MMB----%d\n",zone->mmb_nums);
#endif
}
#endif
#endif

hil_mmz_t *hil_mmz_create(const char *name, unsigned long gfp, unsigned long phys_start, unsigned long nbytes)
{
    hil_mmz_t *p;

    mmz_trace_func();

    if (name == NULL) {
        printk(KERN_ERR "%s: 'name' can not be zero!", __FUNCTION__);
        return NULL;
    }

    p = kmalloc(sizeof(hil_mmz_t)+1, GFP_KERNEL);

    memset(p, 0, sizeof(hil_mmz_t)+1);
    strlcpy(p->name, name, HIL_MMZ_NAME_LEN);
    p->gfp = gfp;
    p->phys_start = phys_start;
    p->nbytes = nbytes;

    INIT_LIST_HEAD(&p->list);
    INIT_LIST_HEAD(&p->mmb_list);

    p->destructor = kfree;

#if  MMB_ALLOC_OPTIMIZE
    p->alloc_type = ADVANCE_ALLOC;
    INIT_LIST_HEAD(&p->free_list);
#endif

    return p;
}

hil_mmz_t *hil_mmz_create_v2(const char *name, unsigned long gfp, unsigned long phys_start, 
        unsigned long nbytes, unsigned int alloc_type, unsigned long block_align)
{
    hil_mmz_t *p;

    mmz_trace_func();

    if (name == NULL) {
        printk(KERN_ERR "%s: 'name' can not be zero!", __FUNCTION__);
        return NULL;
    }

    p = kmalloc(sizeof(hil_mmz_t), GFP_KERNEL);

    memset(p, 0, sizeof(hil_mmz_t));
    strlcpy(p->name, name, HIL_MMZ_NAME_LEN);
    p->gfp = gfp;
    p->phys_start = phys_start;
    p->nbytes = nbytes;
    p->alloc_type = alloc_type;
    p->block_align = block_align;

    INIT_LIST_HEAD(&p->list);
    INIT_LIST_HEAD(&p->mmb_list);

    p->destructor = kfree;

    return p;
}

int hil_mmz_destroy(hil_mmz_t *zone)
{
    if (zone == NULL)
        return -1;
    if (zone->destructor)
        zone->destructor(zone);
    return 0;
}

static int _check_mmz(hil_mmz_t *zone)
{
    hil_mmz_t *p;

    unsigned long new_start=zone->phys_start;
    unsigned long new_end=zone->phys_start+zone->nbytes;

    if (zone->nbytes == 0)
        return -1;

    if (!((new_start>=__pa(high_memory)) || (new_start<PHYS_OFFSET && new_end<=PHYS_OFFSET))) {
        printk(KERN_ERR "ERROR: Conflict MMZ:\n");
        printk(KERN_ERR HIL_MMZ_FMT_S "\n", hil_mmz_fmt_arg(zone));
        printk(KERN_ERR "MMZ conflict to kernel memory (0x%08lX, 0x%08lX)\n", (long unsigned int)PHYS_OFFSET, __pa(high_memory)-1);

        return -1;
    }

    list_for_each_entry(p,&mmz_list, list) {
        unsigned long start,end;

        start = p->phys_start;
        end   = p->phys_start + p->nbytes;

        if (new_start >= end) {
            continue;
        } else if (new_start<start && new_end<=start) {
            continue;
        } else {}

        printk(KERN_ERR "ERROR: Conflict MMZ:\n");
        printk(KERN_ERR "MMZ new:   " HIL_MMZ_FMT_S "\n", hil_mmz_fmt_arg(zone));
        printk(KERN_ERR "MMZ exist: " HIL_MMZ_FMT_S "\n", hil_mmz_fmt_arg(p));
        printk(KERN_ERR "Add new MMZ failed!\n");
        return -1;
    }

    return 0;
}

int hil_mmz_register(hil_mmz_t *zone)
{
    int ret = 0;

    //mmz_trace(1, HIL_MMZ_FMT_S, hil_mmz_fmt_arg(zone));

    if (zone == NULL)
        return -1;

    down(&mmz_lock);

    ret = _check_mmz(zone);
    if (ret) {
        up(&mmz_lock);
        return ret;
    }
#if  MMB_ALLOC_OPTIMIZE
    if (zone->alloc_type == ADVANCE_ALLOC) {
        unsigned long prev_area_size = 0;
        unsigned int idx = 0;
        struct mmb_free_node *p_free_nodes = NULL;

        if (check_page_area_percent()) {
            printk(KERN_ERR "Please check you perent define!\n");
            up(&mmz_lock);
            return 1; 
        }
           
        zone->bitmap = NULL;
        zone->nodes_num = ALLOC_PAGES_MAX_NUM;
        INIT_LIST_HEAD(&zone->free_list);

        for (idx = 0; idx < ALLOC_PAGES_MAX_NUM; idx++) {           
            if (idx == 0)  {
                zone->alloc_start_addr[idx]  = zone->phys_start;  
            } else {
                zone->alloc_start_addr[idx] = zone->alloc_start_addr[idx - 1] + prev_area_size ;
            } 
            
            p_free_nodes = kmalloc(sizeof(struct mmb_free_node), GFP_KERNEL);
            p_free_nodes->addr_start = zone->alloc_start_addr[idx];
            p_free_nodes->mmb_size = mmz_grain_align((zone->nbytes/AREA_PER_ALL) * s_alloc_pages_area[idx].percent);

            //printk(KERN_INFO "New node :%d. [%lx, %lx].%lx\n",idx, p_free_nodes->addr_start,p_free_nodes->mmb_size,zone->nbytes);
            
            list_add(&p_free_nodes->list, zone->free_list.prev); //the head previous is the last node  

            zone->alloc_start_node[idx] = &(p_free_nodes->list);                
            prev_area_size = p_free_nodes->mmb_size;
        }               
    }
    else
#endif
    {
        zone->bitmap = kmalloc(mmz_bitmap_size(zone), GFP_KERNEL);
        memset(zone->bitmap, 0, mmz_bitmap_size(zone));
    }

    INIT_LIST_HEAD(&zone->mmb_list);

#if  MMB_LIST_OPTIMIZE
    zone->mmb_sec_area_start = &zone->mmb_list;
    zone->mmb_nums = 0;
#endif
    list_add(&zone->list, &mmz_list);

    up(&mmz_lock);

    return 0;
}

int hil_mmz_unregister(hil_mmz_t *zone)
{
    int losts = 0;
    hil_mmb_t *p;

    if (zone == NULL)
        return -1;

    mmz_trace_func();

    down(&mmz_lock);
    list_for_each_entry(p,&zone->mmb_list, list) {
        printk(KERN_WARNING "          MB Lost: " HIL_MMB_FMT_S "\n", hil_mmb_fmt_arg(p));
        losts++;
    }

    if (losts) {
        printk(KERN_ERR "%d mmbs not free, mmz<%s> can not be deregistered!\n", losts, zone->name);
        up(&mmz_lock);
        return -1;
    }

    list_del(&zone->list);
#if  MMB_ALLOC_OPTIMIZE
    if (zone->alloc_type == ADVANCE_ALLOC) {
         struct mmb_free_node *p_node,*n_node;

         p_node = list_entry(zone->free_list.next, struct mmb_free_node, list);
         n_node = list_entry(p_node->list.next, struct mmb_free_node, list);
         for (;;) {     
             kfree(p_node);     
             p_node = n_node;
             if (&p_node->list == &zone->free_list)   break;
             else n_node =  list_entry(n_node->list.next, struct  mmb_free_node, list);
         }
    }
#endif
    kfree(zone->bitmap);
    zone->bitmap = NULL;
    //hil_mmz_destroy(zone);
    up(&mmz_lock);

    return 0;
}

static unsigned long _find_fixed_region(unsigned long *region_len, hil_mmz_t *mmz,
        unsigned long size, unsigned long align)
{
    int i;
    unsigned long fixed_start = 0;
    unsigned long fixed_len = ~1;

    mmz_trace_func();

#if  MMB_ALLOC_OPTIMIZE
    if (mmz->alloc_type == ADVANCE_ALLOC)    {
         struct mmb_free_node *p_node = NULL;
         
         int area_idx;
         area_idx = get_page_area_index(mmz_length2grain(size));

         p_node = list_entry(mmz->alloc_start_node[area_idx], struct mmb_free_node, list);
         for (; &p_node->list != &mmz->free_list; p_node = list_entry(p_node->list.next, struct mmb_free_node, list)) {
             if (p_node->mmb_size == mmz_grain_align(size)) {
                 fixed_start = p_node->addr_start;
                 fixed_len = p_node->mmb_size;
                 //the first node
                 for (i = 0; i < ALLOC_PAGES_MAX_NUM; i++) {
                     if (mmz->alloc_start_node[i] == &p_node->list) {
                         mmz->alloc_start_node[i] = p_node->list.next;
                     }                    
                 }
                 list_del(&p_node->list);
                 kfree(p_node);
                 break;
             } else if (p_node->mmb_size > mmz_grain_align(size)) {
                 fixed_start = p_node->addr_start;
                 fixed_len = mmz_grain_align(size);
                 p_node->addr_start += fixed_len;
                 p_node->mmb_size -= fixed_len; 
                 break;
              }
         }

         if (&p_node->list == &mmz->free_list) {
              //printk(KERN_ERR "-------------Can not be fixed region---------------!\n");
              //printk(KERN_INFO "fix.area:%d. size:%lx\n", area_idx, mmz_grain_align(size));
              p_node = list_entry(mmz->free_list.prev, struct mmb_free_node, list);
              //printk(KERN_INFO "free.start:%lx. size:%lx\n", p_node->addr_start, p_node->mmb_size);
              //failed!             
         }
    } else {
#endif
    /* align to phys address first! */
    i = mmz_phy_addr2pos(mmz, mmz_align2(mmz->phys_start, align));
    align = mmz_grain_align(align)/MMZ_GRAIN;

    for (; i<mmz_length2grain(mmz->nbytes); i+=align) {
        unsigned long start;
        unsigned long len;

        if (mmz_get_bit(mmz,i))
            continue;

        len = 0;
        start = mmz_pos2phy_addr(mmz,i);
        for (; i<mmz_length2grain(mmz->nbytes); i++) {
            if (mmz_get_bit(mmz,i)) {
                //i = mmz_align2(i,align);
                i = mmz_phy_addr2pos(mmz,mmz_align2(mmz_pos2phy_addr(mmz,i), align)) - align;
                break;
            }

            len += MMZ_GRAIN;

            //when get it, go out
            if ((len<fixed_len) && (len>=size)) {
                fixed_len = len;
                fixed_start = start;
                //mmz_trace(1,"fixed_region: start=0x%08lX, len=%luKB", fixed_start, fixed_len/SZ_1K);
                goto out;
            }

        }

        //mmz_trace(1,"      region: start=0x%08lX, len=%luKB", start, len/SZ_1K);
        /*
        if ((len<fixed_len) && (len>=size)) {
            fixed_len = len;
            fixed_start = start;
            mmz_trace(1,"fixed_region: start=0x%08lX, len=%luKB", fixed_start, fixed_len/SZ_1K);
        }
        */
    }
#if  MMB_ALLOC_OPTIMIZE
    }
#endif    

    out:
    *region_len = fixed_len;

    return fixed_start;
}

static unsigned long _find_fixed_region_from_highaddr(unsigned long *region_len, hil_mmz_t *mmz,
        unsigned long size, unsigned long align)
{
    int i, j;
    unsigned long fixed_start=0;
    unsigned long fixed_len=~1;

    mmz_trace_func();

    i = mmz_length2grain(mmz->nbytes);

    for (; i>0; i--) {
        unsigned long start;
        unsigned long len;
        unsigned long start_highaddr;
        
        if (mmz_get_bit(mmz,i))
            continue;

        len = 0;
        start_highaddr = mmz_pos2phy_addr(mmz,i);
        for (; i>0; i--) {
            if (mmz_get_bit(mmz,i)) {
                break;
            }

            len += MMZ_GRAIN;
        }

        if (len>=size) {
            j = mmz_phy_addr2pos(mmz, mmz_align2low(start_highaddr-size, align));
            //align = mmz_grain_align(align)/MMZ_GRAIN;
            //start = mmz_pos2phy_addr(mmz, j - align);
            start = mmz_pos2phy_addr(mmz, j);
            if ((start_highaddr - len <= start) && (start <= start_highaddr - size)) {
                fixed_len = len;
                fixed_start = start;
                break;
            }                
                        
            //mmz_trace(1,"fixed_region: start=0x%08lX, len=%luKB", fixed_start, fixed_len/SZ_1K);
        }
    }

    *region_len = fixed_len;

    return fixed_start;
}

static int _do_mmb_alloc(hil_mmb_t *mmb)
{
    int i;
    hil_mmb_t *p = NULL;

#if  MMB_ALLOC_OPTIMIZE
#if  MMB_LIST_OPTIMIZE
    int flag = 0;
    struct list_head  *p_start = NULL;
    struct list_head  *p_end   = NULL;
#endif   
    if  (mmb->zone->alloc_type != ADVANCE_ALLOC)    
#endif
    for (i = 0; i < mmz_length2grain(mmb->length); i++)
        mmz_set_bit(mmb->zone,i+mmz_phy_addr2pos(mmb->zone,mmb->phys_addr));

#if  MMB_LIST_OPTIMIZE    
    if  (mmb->zone->mmb_nums >= MMB_NUMS_INFLEXTION) {
        mmb->zone->mmb_nums++;
        if  (mmb->phys_addr >= mmb->zone->alloc_start_addr[(ALLOC_PAGES_MAX_NUM + 1) /2]) {       
            p_start = mmb->zone->mmb_sec_area_start;
            p_end = mmb->zone->mmb_list.prev;
    
            if  (p_start == &mmb->zone->mmb_list || p_end == &mmb->zone->mmb_list) 
                list_add(&mmb->list, mmb->zone->mmb_list.prev);
            else if  (mmb->phys_addr >= mmb->zone->alloc_start_addr[(ALLOC_PAGES_MAX_NUM + 1) * 3 /4]) 
            	    flag = 2; 
            else
            	    flag = 1; 
        } else {       
            p_start = mmb->zone->mmb_list.next;
            p_end = mmb->zone->mmb_sec_area_start->prev;
               
            if  (p_start == &mmb->zone->mmb_list) 
            	    list_add(&mmb->list, &mmb->zone->mmb_list);
            else if (p_end == &mmb->zone->mmb_list) {
                //list_add(&mmb->list, &mmb->zone->mmb_list); 
                printk(KERN_ERR "ERROR: logic is not right!\n");    
                flag = 3; 
            }
            else if  (mmb->phys_addr >= mmb->zone->alloc_start_addr[(ALLOC_PAGES_MAX_NUM + 1) /4]) 
            	    flag = 2;      
            else 
            	    flag = 1;
        }

        if(p_start == NULL || p_end == NULL) {
             printk(KERN_ERR "ERROR: p_start or p_end is NULL!\n");  
             flag = 3; 
         }
    
        if  (flag == 1) {
             p = list_entry(p_start, hil_mmb_t, list);
             for (;;) {
                if  (mmb->phys_addr < p->phys_addr) {
                    list_add(&mmb->list, p->list.prev);
                    break;
                }
                
                if (mmb->phys_addr == p->phys_addr) {
                    printk(KERN_ERR "ERROR: media-mem allocator bad in %s! (%s, %d)",
                            mmb->zone->name[0]?mmb->zone->name:"----",  __FUNCTION__, __LINE__);
                    //break;
                }
    
                if (&p->list == p_end || p->list.next == &mmb->zone->mmb_list) {
                        list_add(&mmb->list, &p->list);
                        break;
                }
                
                p = list_entry(p->list.next, hil_mmb_t, list);
            }        
        } else if (flag == 2) {
             p = list_entry(p_end, hil_mmb_t, list);
             for (;;) {
                if (mmb->phys_addr > p->phys_addr) {
                    list_add(&mmb->list, &p->list);
                    break;
                }
                
                if (mmb->phys_addr == p->phys_addr) {
                    printk(KERN_ERR "ERROR: media-mem allocator bad in %s! [%s : (%lu,%lu), %s : (%lu,%lu)][%s, %d]",
                            mmb->zone->name[0]?mmb->zone->name:"----", mmb->name, mmb->phys_addr, mmb->length, p->name, p->phys_addr, p->length, __FUNCTION__, __LINE__);
                    //break;
                }
                
                if (&p->list == p_start || p->list.prev == &mmb->zone->mmb_list) {
                        list_add(&mmb->list, p->list.prev);
                        break;
                }   
                
                p = list_entry(p->list.prev, hil_mmb_t, list);
            }
        } else if (flag == 3) {
            //int sort_ok  = 1;
            list_for_each_entry(p,&mmb->zone->mmb_list, list) {
                if (mmb->phys_addr < p->phys_addr) {
                    break;
                	}
                
                if (mmb->phys_addr == p->phys_addr) {
                    printk(KERN_ERR "ERROR: media-mem allocator bad in %s! (%s, %d)",
                            mmb->zone->name[0]?mmb->zone->name:"----",  __FUNCTION__, __LINE__);
#if 0                    
                    p->length = mmb->length;
                    p->kvirt = mmb->kvirt;
                    p->map_ref = mmb->map_ref;                    
                    sort_ok  = 0;                  
                    break;
#endif                      
                }
            } 
            
            //if(sort_ok) {
                list_add(&mmb->list, p->list.prev);
            //}            
        }
    } else {
        /* add mmb sorted */
        //int sort_ok  = 1;
        list_for_each_entry(p,&mmb->zone->mmb_list, list) {
            if (mmb->phys_addr < p->phys_addr) {
                break;
            	}
            
            if (mmb->phys_addr == p->phys_addr) {
                printk(KERN_ERR "ERROR: media-mem allocator bad in %s! (%s, %d)",
                        mmb->zone->name[0]?mmb->zone->name:"----",  __FUNCTION__, __LINE__);
#if 0                
                p->length = mmb->length;
                p->kvirt = mmb->kvirt;
                p->map_ref = mmb->map_ref;
                sort_ok  = 0;
                break;
#endif                
            }
        }   
        
        //if(sort_ok) {
            list_add(&mmb->list, p->list.prev);
        //}
    }

    if (mmb->phys_addr >= mmb->zone->alloc_start_addr[(ALLOC_PAGES_MAX_NUM + 1) /2]) {
        if (mmb->zone->mmb_sec_area_start == &mmb->zone->mmb_list) {
                mmb->zone->mmb_sec_area_start = &mmb->list;
        } else {
                p = list_entry(mmb->zone->mmb_sec_area_start, hil_mmb_t, list);
                if (p->phys_addr > mmb->phys_addr) {
                    mmb->zone->mmb_sec_area_start = &mmb->list;
                }
        }
    }     
#else
    /* add mmb sorted */
    list_for_each_entry(p,&mmb->zone->mmb_list, list) {
        if (mmb->phys_addr < p->phys_addr)
            break;
        if (mmb->phys_addr == p->phys_addr) {
            printk(KERN_ERR "ERROR: media-mem allocator bad in %s! (%s, %d)",
                    mmb->zone->name[0]?mmb->zone->name:"----",  __FUNCTION__, __LINE__);
        }
    }
    list_add(&mmb->list, p->list.prev);
#endif    
    //mmz_trace(1,HIL_MMB_FMT_S,hil_mmb_fmt_arg(mmb));

    return 0;
}

static hil_mmb_t *__mmb_alloc(const char *name, unsigned long size, unsigned long align, 
        unsigned long gfp, const char *mmz_name, hil_mmz_t *_user_mmz)
{
    hil_mmz_t *mmz;
    hil_mmb_t *mmb;

    unsigned long start;
    unsigned long region_len;

    unsigned long fixed_start=0;
    unsigned long fixed_len=~1;
    hil_mmz_t *fixed_mmz = NULL;

    mmz_trace_func();

    if (size == 0 || size > 0x40000000UL)
        return NULL;
    if (align == 0)
        align = 1;

    size = mmz_grain_align(size);

    //mmz_trace(1,"size=%luKB, align=%lu", size/SZ_1K, align);

    begin_list_for_each_mmz(mmz, gfp, mmz_name)
        if (_user_mmz != NULL && _user_mmz != mmz)
            continue;
        start = _find_fixed_region(&region_len, mmz, size, align);
        if ( (fixed_len > region_len) && (start != 0)) {
            fixed_len = region_len;
            fixed_start = start;
            fixed_mmz = mmz;
        }
    end_list_for_each_mmz()

    if (fixed_mmz == NULL) {
        return NULL;
    }

    mmb = kmalloc(sizeof(hil_mmb_t), GFP_KERNEL);

    if (mmb == NULL) {
        printk(KERN_WARNING "__mmb_alloc struct error\n");
        return NULL;
    }    

    memset(mmb, 0, sizeof(hil_mmb_t));
    mmb->zone = fixed_mmz;
    mmb->phys_addr = fixed_start;
    mmb->length = size;
    if (name)
        strlcpy(mmb->name, name, HIL_MMB_NAME_LEN);
    else strcpy(mmb->name, "<null>");

    if (_do_mmb_alloc(mmb)) {
        kfree(mmb);
        mmb = NULL;
    }

#if  MMZ_OPTIMIZE_DEBUG
     if(mmz_list_check(mmb->zone, 1, 1)) {
         printk(KERN_INFO "----alloc[start: 0x%08lX. size: 0x%08lX]\n", mmb->phys_addr, mmb->length);        
         mmz_optimize_debug(mmb->zone, 1, 1, 1);
         printk(KERN_INFO "\n");
     }
#endif           
    
    return mmb;
}

static hil_mmb_t *__mmb_alloc_v2(const char *name, unsigned long size, unsigned long align, unsigned long gfp, 
            const char *mmz_name, hil_mmz_t *_user_mmz, unsigned int order)
{
    hil_mmz_t *mmz;
    hil_mmb_t *mmb;
    int i;

    unsigned long start = 0;
    unsigned long region_len;

    unsigned long fixed_start=0;
    unsigned long fixed_len=~1;
    hil_mmz_t *fixed_mmz=NULL;

    mmz_trace_func();

    if (size == 0 || size > 0x40000000UL)
        return NULL;
    if (align == 0)
        align = 1;
        
    size = mmz_grain_align(size);

    //mmz_trace(1,"size=%luKB, align=%lu", size/SZ_1K, align);

    begin_list_for_each_mmz(mmz, gfp, mmz_name)
        if (_user_mmz!=NULL && _user_mmz!=mmz)
            continue;
            
        if (mmz->alloc_type == SLAB_ALLOC) {
            if ((size-1) & size) {
                for (i = 1; i <= 32; i++) {
                    if (!((size >> i) & ~0)) {
                        size = 1 << i;    
                        break;
                    }                        
                }    
                    
            }                
        }
        else if (mmz->alloc_type == EQ_BLOCK_ALLOC) {
            size = mmz_align2(size,mmz->block_align);
        }    
            
            
        if (order == LOW_TO_HIGH) {            
            start = _find_fixed_region(&region_len, mmz, size, align);
        }
        else if (order == HIGH_TO_LOW)
            start = _find_fixed_region_from_highaddr(&region_len, mmz, size, align);
        if ( (fixed_len > region_len) && (start!=0)) {
            fixed_len = region_len;
            fixed_start = start;
            fixed_mmz = mmz;
        }
    end_list_for_each_mmz()

    if (fixed_mmz == NULL) {
        return NULL;
    }

    mmb = kmalloc(sizeof(hil_mmb_t), GFP_KERNEL);

    memset(mmb, 0, sizeof(hil_mmb_t));
    mmb->zone = fixed_mmz;
    mmb->phys_addr = fixed_start;
    mmb->length = size;
    mmb->order = order;
    if (name)
        strlcpy(mmb->name, name, HIL_MMB_NAME_LEN);
    else strcpy(mmb->name, "<null>");

    if (_do_mmb_alloc(mmb)) {
        kfree(mmb);
        mmb = NULL;
    }

    return mmb;
}

hil_mmb_t *hil_mmb_alloc(const char *name, unsigned long size, unsigned long align, 
        unsigned long gfp, const char *mmz_name)
{
    hil_mmb_t *mmb;

    down(&mmz_lock);
    mmb = __mmb_alloc(name, size, align, gfp, mmz_name, NULL);
    up(&mmz_lock);

    return mmb;
}

hil_mmb_t *hil_mmb_alloc_v2(const char *name, unsigned long size, unsigned long align, 
        unsigned long gfp, const char *mmz_name, unsigned int order)
{
    hil_mmb_t *mmb;

    down(&mmz_lock);
    mmb = __mmb_alloc_v2(name, size, align, gfp, mmz_name, NULL, order);
    up(&mmz_lock);

    return mmb;
}

hil_mmb_t *hil_mmb_alloc_in(const char *name, unsigned long size, unsigned long align, 
        hil_mmz_t *_user_mmz)
{
    hil_mmb_t *mmb;

    if (_user_mmz==NULL)
        return NULL;

    down(&mmz_lock);
    mmb = __mmb_alloc(name, size, align, _user_mmz->gfp, _user_mmz->name, _user_mmz);
    up(&mmz_lock);

    return mmb;
}

hil_mmb_t *hil_mmb_alloc_in_v2(const char *name, unsigned long size, unsigned long align, 
        hil_mmz_t *_user_mmz, unsigned int order)
{
    hil_mmb_t *mmb;

    if (_user_mmz==NULL)
        return NULL;

    down(&mmz_lock);
    mmb = __mmb_alloc_v2(name, size, align, _user_mmz->gfp, _user_mmz->name, _user_mmz, order);
    up(&mmz_lock);

    return mmb;
}

static void *_mmb_map2kern(hil_mmb_t *mmb, int cached)
{    
    if (mmb->flags & HIL_MMB_MAP2KERN) {
        if ((cached*HIL_MMB_MAP2KERN_CACHED) != (mmb->flags&HIL_MMB_MAP2KERN_CACHED)) {
            printk(KERN_ERR "mmb<%s> already kernel-mapped %s, can not be re-mapped as %s.",
                    mmb->name,
                    (mmb->flags&HIL_MMB_MAP2KERN_CACHED) ? "cached" : "non-cached",
                    (cached) ? "cached" : "non-cached" );
            return NULL;
        }

        mmb->map_ref++;

        return mmb->kvirt;
    }

    if (cached) {
        mmb->flags |= HIL_MMB_MAP2KERN_CACHED;
        mmb->kvirt = ioremap_cached(mmb->phys_addr, mmb->length);
    } else {
        mmb->flags &= ~HIL_MMB_MAP2KERN_CACHED;
        mmb->kvirt = ioremap_nocache(mmb->phys_addr, mmb->length);
    }

    if (mmb->kvirt) {
               mmb->flags |= HIL_MMB_MAP2KERN;
        mmb->map_ref++;
    }

    return mmb->kvirt;
}

void *hil_mmb_map2kern(hil_mmb_t *mmb)
{
    void *p;
     
    if (mmb == NULL)
        return NULL;
    


    down(&mmz_lock);
    p =  _mmb_map2kern(mmb, 0);
    up(&mmz_lock);

    return p;
}

void *hil_mmb_map2kern_cached(hil_mmb_t *mmb)
{
    void *p;
    
    if (mmb == NULL)
        return NULL;

    mmz_trace_func();    

    down(&mmz_lock);
    p = _mmb_map2kern(mmb, 1);
    up(&mmz_lock);

    return p;
}

static int _mmb_free(hil_mmb_t *mmb);

int hil_mmb_unmap(hil_mmb_t *mmb)
{
    int ref;

    if (mmb == NULL)
        return -1;
    
    down(&mmz_lock);

    if  (mmb->flags & HIL_MMB_MAP2KERN_CACHED)
    {
	__cpuc_flush_dcache_area((void *)mmb->kvirt, (size_t)mmb->length);
	outer_flush_range(mmb->phys_addr, mmb->phys_addr + mmb->length);
    }

    if (mmb->flags & HIL_MMB_MAP2KERN) {
        ref = --mmb->map_ref;
        if (mmb->map_ref !=0) {
            up(&mmz_lock);
            return ref;
        }

        iounmap(mmb->kvirt);
    }

    mmb->kvirt = NULL;
    mmb->flags &= ~HIL_MMB_MAP2KERN;
    mmb->flags &= ~HIL_MMB_MAP2KERN_CACHED;

    if ((mmb->flags & HIL_MMB_RELEASED) && mmb->phy_ref == 0) {
        _mmb_free(mmb);
    }

    up(&mmz_lock);

    return 0;
}

int hil_mmb_get(hil_mmb_t *mmb)
{
    int ref;

    if (mmb == NULL)
        return -1;
  
    down(&mmz_lock);

    if (mmb->flags & HIL_MMB_RELEASED)
        printk(KERN_WARNING "hil_mmb_get: amazing, mmb<%s> is released!\n", mmb->name);
    ref = ++mmb->phy_ref;

    up(&mmz_lock);

    return ref;
}

static int _mmb_free(hil_mmb_t *mmb)
{
    int i;
#if  MMB_ALLOC_OPTIMIZE
    if (mmb->zone->alloc_type == ADVANCE_ALLOC) {
        unsigned int area_idx = 0;
        unsigned int idx = 0;   
        unsigned long next_addr = 0;
        struct list_head *p_start = NULL;
        struct list_head *p_last= NULL;
        struct list_head *p_prev = NULL;
        struct list_head *p_next= NULL;      
        struct mmb_free_node *temp_node = NULL;
        struct mmb_free_node *temp_node2 = NULL;
        struct mmb_free_node *new_node = NULL;
 
        for (idx = 0; idx < (ALLOC_PAGES_MAX_NUM - 1); idx++) {
            if (mmb->phys_addr >= mmb->zone->alloc_start_addr[idx] && mmb->phys_addr < mmb->zone->alloc_start_addr[idx+1]) {
                   break;
            }               
        }
        area_idx = idx;
 
        p_start= mmb->zone->alloc_start_node[area_idx];
 
        if (area_idx == (ALLOC_PAGES_MAX_NUM - 1)) {
            p_last = mmb->zone->free_list.prev;
            next_addr = mmb->zone->phys_start + mmb->zone->nbytes;
        } else {
            p_last = mmb->zone->alloc_start_node[area_idx  + 1]->prev;
            next_addr = mmb->zone->alloc_start_addr[idx + 1];
        }
         
        if ((list_entry(p_start, struct mmb_free_node, list))->addr_start >= next_addr) {
            //former alloc is overlow and point next area,then insert a node in this area
            new_node = kmalloc(sizeof(struct mmb_free_node), GFP_KERNEL);
            new_node->addr_start = mmb->phys_addr;
            new_node->mmb_size = mmb->length;
            list_add(&new_node->list, p_start->prev);
            for (i = 0; i <= area_idx; i++) {
                if (mmb->zone->alloc_start_node[i] == p_start) {
                    mmb->zone->alloc_start_node[i] = &new_node->list;
                }
            }
            mmb->zone->nodes_num++;            
        } else if ((list_entry(p_start, struct  mmb_free_node, list))->addr_start > mmb->phys_addr) {
            //before first node 
            temp_node = list_entry(p_start, struct  mmb_free_node, list);
            if ((mmb->phys_addr + mmb->length) == temp_node->addr_start) {
                //compose this mmb size to first node
                temp_node->addr_start = mmb->phys_addr;
                temp_node->mmb_size += mmb->length;
            } else {
                //insert a node before first
                new_node = kmalloc(sizeof(struct mmb_free_node), GFP_KERNEL);
                new_node->addr_start = mmb->phys_addr;
                new_node->mmb_size = mmb->length;
                list_add(&new_node->list, p_start->prev);
                for(i = 0; i <= area_idx; i++) {
                    if(mmb->zone->alloc_start_node[i] == p_start) {
                        mmb->zone->alloc_start_node[i] = &new_node->list;
                    }
                }                
                mmb->zone->nodes_num++;
            }
        } else if ((list_entry(p_last, struct mmb_free_node, list))->addr_start < mmb->phys_addr) {
           //after last node
           temp_node = list_entry(p_last, struct  mmb_free_node, list);
           if (mmb->phys_addr == (temp_node->addr_start + temp_node->mmb_size)) {
                //compose this mmb size to last node
                temp_node->mmb_size += mmb->length;
            } else {
                //insert a node after last
                new_node = kmalloc(sizeof(struct mmb_free_node), GFP_KERNEL);
                new_node->addr_start = mmb->phys_addr;
                new_node->mmb_size = mmb->length;
                list_add(&new_node->list, p_last);
                mmb->zone->nodes_num++;
            }               
        }  else {
            p_next = p_last;
            for (p_prev = p_last->prev; ; p_prev = p_prev->prev) {
                 if ( (list_entry(p_prev, struct  mmb_free_node, list))->addr_start < mmb->phys_addr) {
                      p_next = p_prev->next;          
                      break;
                 }
            }
 
            temp_node = list_entry(p_prev, struct  mmb_free_node, list);
            temp_node2 = list_entry(p_next, struct mmb_free_node, list);
            if (mmb->phys_addr == (temp_node->addr_start + temp_node->mmb_size) && 
                (mmb->phys_addr + mmb->length) == temp_node2->addr_start) { 
                temp_node->mmb_size += mmb->length + temp_node2->mmb_size;
                list_del(&temp_node2->list);
                kfree(temp_node2);
                mmb->zone->nodes_num--;
            } else if (mmb->phys_addr == (temp_node->addr_start + temp_node->mmb_size)) {
                temp_node->mmb_size += mmb->length;
            } else if ((mmb->phys_addr + mmb->length) == temp_node2->addr_start) {
                temp_node2->addr_start = mmb->phys_addr;
                temp_node2->mmb_size += mmb->length;
            } else {
                new_node = kmalloc(sizeof(struct mmb_free_node), GFP_KERNEL);
                new_node->addr_start = mmb->phys_addr;
                new_node->mmb_size = mmb->length;
                list_add(&new_node->list, p_prev);       
                mmb->zone->nodes_num++;
            }
        }    
    } else
#endif    
    for (i = 0; i < mmz_length2grain(mmb->length); i++)
        mmz_clr_bit(mmb->zone,i+mmz_phy_addr2pos(mmb->zone,mmb->phys_addr));

    if  (mmb->flags & HIL_MMB_MAP2KERN_CACHED)
    {
	__cpuc_flush_dcache_area((void *)mmb->kvirt, (size_t)mmb->length);
	outer_flush_range(mmb->phys_addr, mmb->phys_addr + mmb->length);
    }
#if  MMB_LIST_OPTIMIZE
    mmb->zone->mmb_nums--;
    if (&mmb->list == mmb->zone->mmb_sec_area_start) {
        mmb->zone->mmb_sec_area_start = mmb->list.next;
    }
#endif
    list_del(&mmb->list);
#if  MMZ_OPTIMIZE_DEBUG
    if(mmz_list_check(mmb->zone, 1, 1)) {
         printk(KERN_INFO "---free[start: 0x%08lX. size: 0x%08lX]\n", mmb->phys_addr, mmb->length);        
         mmz_optimize_debug(mmb->zone, 1, 1, 1);
         printk(KERN_INFO "\n");         
    }
#endif  
    kfree(mmb);
    mmb = NULL;
   
    return 0;
}

int hil_mmb_put(hil_mmb_t *mmb)
{
    int ref;

    if (mmb == NULL)
        return -1;


    
    down(&mmz_lock);

    if (mmb->flags & HIL_MMB_RELEASED)
        printk(KERN_WARNING "hil_mmb_put: amazing, mmb<%s> is released!\n", mmb->name);

    ref = --mmb->phy_ref;
    
    if ((mmb->flags & HIL_MMB_RELEASED) && mmb->phy_ref == 0 && mmb->map_ref == 0) {
        _mmb_free(mmb);
    }

    up(&mmz_lock);

    return ref;
}

int hil_mmb_force_put(hil_mmb_t *mmb)
{
    if (mmb == NULL)
        return -1;
    
    down(&mmz_lock);

    if (mmb->flags & HIL_MMB_RELEASED)
        printk(KERN_WARNING "hil_mmb_put: amazing, mmb<%s> is released!\n", mmb->name);

    mmb->phy_ref = 0;
    
    if ((mmb->flags & HIL_MMB_RELEASED) && mmb->phy_ref == 0 && mmb->map_ref == 0) {
        _mmb_free(mmb);
    }

    up(&mmz_lock);

    return 0;
}
int hil_mmb_free(hil_mmb_t *mmb)
{
    mmz_trace_func();

    if (mmb == NULL)
        return -1;

    //mmz_trace(1,HIL_MMB_FMT_S,hil_mmb_fmt_arg(mmb));

    down(&mmz_lock);

    if (mmb->flags & HIL_MMB_RELEASED) {
        printk(KERN_WARNING "hil_mmb_free: amazing, mmb<%s> is released before, but still used!\n", mmb->name);

        up(&mmz_lock);

        return 0;
    }

    if (mmb->phy_ref > 0) {
        printk(KERN_WARNING "hil_mmb_free: free mmb<%s, 0x%08lx> delayed for which ref-count is %d ,%d!\n",
                mmb->name, mmb->phys_addr, mmb->map_ref, mmb->phy_ref);
        mmb->flags |= HIL_MMB_RELEASED;
        up(&mmz_lock);

        return 0;
    }

    if (mmb->flags & HIL_MMB_MAP2KERN) {
        printk(KERN_WARNING "hil_mmb_free: free mmb<%s> delayed for which is kernel-mapped to 0x%p with map_ref %d!\n",
                mmb->name, mmb->kvirt, mmb->map_ref);
        mmb->flags |= HIL_MMB_RELEASED;
        up(&mmz_lock);

        return 0;
    }

    _mmb_free(mmb);
    
    up(&mmz_lock);

    return 0;
}

#define MACH_MMB(p, val, member) do{\
                    hil_mmz_t *__mach_mmb_zone__; \
                    (p) = NULL;\
                    list_for_each_entry(__mach_mmb_zone__,&mmz_list, list) { \
                        hil_mmb_t *__mach_mmb__;\
                        list_for_each_entry(__mach_mmb__,&__mach_mmb_zone__->mmb_list, list) { \
                            if (__mach_mmb__->member == (val)) { \
                                (p) = __mach_mmb__; \
                                break;\
                            } \
                        } \
                        if (p)break;\
                    } \
                }while(0)

hil_mmb_t *hil_mmb_getby_phys(unsigned long addr)
{
    hil_mmb_t *p;



    down(&mmz_lock);
    MACH_MMB(p, addr, phys_addr);
    up(&mmz_lock);

    return p;
}

hil_mmb_t *hil_mmb_getby_kvirt(void *virt)
{
    hil_mmb_t *p;

    if (virt == NULL)
        return NULL;


    
    down(&mmz_lock);
    MACH_MMB(p, virt, kvirt);
    up(&mmz_lock);

    return p;
}


#define MACH_MMB_2(p, val, Outoffset) do{\
                    hil_mmz_t *__mach_mmb_zone__; \
                    (p) = NULL;\
                    list_for_each_entry(__mach_mmb_zone__,&mmz_list, list) { \
                        hil_mmb_t *__mach_mmb__;\
                        list_for_each_entry(__mach_mmb__,&__mach_mmb_zone__->mmb_list, list) { \
                            if  ((__mach_mmb__->phys_addr <= (val)) && ((__mach_mmb__->length + __mach_mmb__->phys_addr) > (val))) { \
                                (p) = __mach_mmb__; \
                                Outoffset = val - __mach_mmb__->phys_addr;\
                                break;\
                            }\
                        } \
                        if (p)break;\
                    } \
                }while(0)



hil_mmb_t *hil_mmb_getby_phys_2(unsigned long addr, unsigned long *Outoffset)
{
    hil_mmb_t *p;

    down(&mmz_lock);
    MACH_MMB_2(p, addr, *Outoffset);
    up(&mmz_lock);
    return p;
}

hil_mmz_t *hil_mmz_find(unsigned long gfp, const char *mmz_name)
{
    hil_mmz_t *p;
    

    
    down(&mmz_lock);
    begin_list_for_each_mmz(p, gfp, mmz_name)
        up(&mmz_lock);
        return p;
    end_list_for_each_mmz()
    up(&mmz_lock);

    return NULL;
}

/*
 * name,gfp,phys_start,nbytes,alloc_type;...
 * All param in hex mode, except name.
 */
static int media_mem_parse_cmdline(char *s)
{
    hil_mmz_t *zone = NULL;
    char *line;

    while( (line = strsep(&s,":")) !=NULL) {
        int i;
        char *argv[6];

        /*
         * FIXME: We got 4 args in "line", formated "argv[0],argv[1],argv[2],argv[3],argv[4]".
         * eg: "<mmz_name>,<gfp>,<phys_start_addr>,<size>,<alloc_type>"
         * For more convenient, "hard code" are used such as "arg[0]", i.e.
         */
        for (i=0; (argv[i] = strsep(&line,",")) != NULL;)
            if (++i == ARRAY_SIZE(argv)) break;
        
        if (i == 4) {
            zone = hil_mmz_create("null",0,0,0);
            strlcpy(zone->name, argv[0], HIL_MMZ_NAME_LEN);
            zone->gfp = _strtoul_ex(argv[1], NULL, 0);
            zone->phys_start = _strtoul_ex(argv[2], NULL, 0);
            zone->nbytes = _strtoul_ex(argv[3], NULL, 0);
        }
        
        else if (i == 6) {
            zone = hil_mmz_create_v2("null",0,0,0,0,0);
            strlcpy(zone->name, argv[0], HIL_MMZ_NAME_LEN);
            zone->gfp = _strtoul_ex(argv[1], NULL, 0);
            zone->phys_start = _strtoul_ex(argv[2], NULL, 0);
            zone->nbytes = _strtoul_ex(argv[3], NULL, 0);
            zone->alloc_type = _strtoul_ex(argv[4], NULL, 0);
            zone->block_align = _strtoul_ex(argv[5], NULL, 0);
        }
        
        else{
            printk(KERN_ERR "MMZ: your parameter num is not correct!\n");    
            continue;
        }
        
        mmz_info_phys_start = zone->phys_start + zone->nbytes - 0x2000;
        if (hil_mmz_register(zone)) {
            printk(KERN_WARNING "Add MMZ failed: " HIL_MMZ_FMT_S "\n", hil_mmz_fmt_arg(zone));
            hil_mmz_destroy(zone);
        }
        zone = NULL;
    }

    return 0;
}

int get_mmz_info_phys_start()
{
    return mmz_info_phys_start;
}

#define MAX_MMZ_INFO_LEN 20*1024
#define CHECK_BUFF_OVERFLOW() \
do{\
	if (p_mmz_info_buf + len - mmz_info_buf > MAX_MMZ_INFO_LEN){\
		printk(KERN_ERR"mmz_info_buff overflow(%d), more than 20k data!\n", p_mmz_info_buf + len - mmz_info_buf);\
		break;\
	};\
}while(0)

int mmz_read_proc(char *page, char **start, off_t off,
                            int count, int *eof, void *data)
{
	unsigned long ret;
	hil_mmz_t *p;
	unsigned long len = 0;
	char * mmz_info_buf = (char *)__get_free_pages(GFP_TEMPORARY, get_order(MAX_MMZ_INFO_LEN));
	char * p_mmz_info_buf = mmz_info_buf;
	memset(mmz_info_buf, 0, MAX_MMZ_INFO_LEN);

	down(&mmz_lock);
	list_for_each_entry(p,&mmz_list, list) {
		hil_mmb_t *mmb;

		len = snprintf(page, count, "+---ZONE: " HIL_MMZ_FMT_S "\n", hil_mmz_fmt_arg(p));
		CHECK_BUFF_OVERFLOW();
		memcpy(p_mmz_info_buf, page, len);
		p_mmz_info_buf += len;
		
		mmz_total_size = p->nbytes / 1024;
		zone_number++;

		list_for_each_entry(mmb,&p->mmb_list, list) {
			len = snprintf(page, count, "   |-MMB: " HIL_MMB_FMT_S "\n", hil_mmb_fmt_arg(mmb));
			CHECK_BUFF_OVERFLOW();
			memcpy(p_mmz_info_buf, page, len);
			p_mmz_info_buf += len;

			mmz_used_size += mmb->length / 1024;
		}
	}

	if (mmz_total_size != 0)
	{
		mmz_free_size = mmz_total_size - mmz_used_size;
		len = snprintf(page, count, "+---MMZ_USE_INFO:\n   mmz_size=%dKB(%dMB) , used=%dKB(%dMB)  ,remain=%dKB(%dMB) , zone_number=%d\n",
				mmz_total_size, mmz_total_size/1024,mmz_used_size, mmz_used_size/1024, mmz_free_size, mmz_free_size/1024,zone_number);
		CHECK_BUFF_OVERFLOW();
		memcpy(p_mmz_info_buf, page, len);
		p_mmz_info_buf += len;
		
		mmz_total_size = 0;
		mmz_used_size = 0;
		mmz_free_size = 0;
		zone_number = 0;
	}

	// transfer info to proc buff page
	if (off +  count > p_mmz_info_buf - mmz_info_buf){
		memcpy(page, mmz_info_buf + off, p_mmz_info_buf - mmz_info_buf - off);
		*eof = 1;
		ret = p_mmz_info_buf - mmz_info_buf - off;
	}else{
		memcpy(page, mmz_info_buf + off, count);
		ret = count;
	}
	*start = page;

	up(&mmz_lock);

	free_pages((unsigned long)mmz_info_buf, get_order(MAX_MMZ_INFO_LEN) );

	return ret;
}

int mmz_write_proc(struct file *file, const char __user *buffer,
                                   unsigned long count, void *data)
{
#if !(0 == HI_PROC_SUPPORT)
    char buf[256];



    if (count >= sizeof(buf)) {
        printk(KERN_ERR "MMZ: your parameter string is too long!\n");
        return -EIO;
    }

    memset(buf, 0, sizeof(buf));
    if  (copy_from_user(buf, buffer, count))
    {
        printk(KERN_ERR "MMZ: copy_from_user failed!\n");    
        return -EIO;
    }
    media_mem_parse_cmdline(buf);
#endif

    return count;
}

#define MMZ_SETUP_CMDLINE_LEN 256

#ifndef MODULE
static char __initdata setup_zones[MMZ_SETUP_CMDLINE_LEN] = "ddr,0,0x86000000,160M" ; // CONFIG_HISILICON_MMZ_DEFAULT;
static int __init parse_kern_cmdline(char *line)
{
    strlcpy(setup_zones, line, sizeof(setup_zones));
    printk("000000------setup_zones=%s------000000\n", setup_zones);
    return 1;
}
__setup("mmz=", parse_kern_cmdline);

#else
static char __initdata setup_zones[MMZ_SETUP_CMDLINE_LEN]={'\0'};
module_param_string(mmz, setup_zones, MMZ_SETUP_CMDLINE_LEN, 0600);
MODULE_PARM_DESC(mmz,"mmz=name,0,start,size,type,eqsize:[others]");
#endif

static void mmz_exit_check(void)
{
    hil_mmz_t *p;
    mmz_trace_func();
    for (p=hil_mmz_find(0,NULL); p!=NULL; p=hil_mmz_find(0,NULL)) {
        printk(KERN_WARNING "MMZ force removed: " HIL_MMZ_FMT_S "\n", hil_mmz_fmt_arg(p));
        hil_mmz_unregister(p);
    }
}

int media_mem_init_0(void)
{
    char *s;
    char *p = NULL;
    char *q;
    int len;

    printk(KERN_INFO "Hisilicon Media Memory Zone Manager state 0\n");

    len = strlen(setup_zones);
    if (len == 0)
    {
        strlcpy(line, saved_command_line, COMMAND_LINE_SIZE);
        q = strstr(line, "mmz=");
        if (q)
        {
            s = strsep(&q, "=");
            if (s)
            {
                p = strsep(&q, " ");
            }

            strlcpy(setup_zones, p, MMZ_SETUP_CMDLINE_LEN);
        }
    }

    media_mem_parse_cmdline(setup_zones);
    return 0;
}

void media_mem_exit_0(void)
{
    mmz_exit_check();
    return;
}

EXPORT_SYMBOL(hil_mmb_alloc);
EXPORT_SYMBOL(hil_mmb_alloc_v2);
EXPORT_SYMBOL(hil_mmb_free);
EXPORT_SYMBOL(hil_mmb_get);
EXPORT_SYMBOL(hil_mmb_put);
EXPORT_SYMBOL(mmz_read_proc);
EXPORT_SYMBOL(mmz_write_proc);
EXPORT_SYMBOL(media_mem_init_0);
EXPORT_SYMBOL(media_mem_exit_0);
EXPORT_SYMBOL(hil_mmb_getby_phys);
EXPORT_SYMBOL(hil_mmb_getby_phys_2);
