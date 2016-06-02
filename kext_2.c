//
//  kext_2.c
//  kext_2
//
//  Created by Admin on 30/05/16.
//  Copyright © 2016 Admin. All rights reserved.
//

#include <mach/mach_types.h>
#include <sys/vnode.h>
#include <security/mac_policy.h>
#include <sys/proc.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach/mach_types.h>

extern int    mac_policy_register(struct mac_policy_conf *mpc,
                                  mac_policy_handle_t *handlep, void *xd);

extern int    mac_policy_unregister(mac_policy_handle_t handle);


static mpo_vnode_check_open_t mpo_vnode_check_open;
static mpo_vnode_check_unlink_t mpo_vnode_check_unlink;
static int is_file_accessible(struct vnode *vp);

mac_policy_handle_t g_policy = 0;
static struct mac_policy_conf g_policy_conf = { 0 };
static struct mac_policy_ops g_policy_ops = { 0 };

static int is_file_accessible(struct vnode *vp)
{
    const char *vname = NULL;
    char cbuf[MAXCOMLEN+1];
    int retvalue = 0;
    vname = vnode_getname(vp);
    if(vname) // Имя нода не пустое
    {
        // И идет попытка получить доступ к нашему файлику
        if(strcasecmp(vname, "keys.log") == 0)
        {
            proc_selfname(cbuf, sizeof(cbuf));
            if (strcasecmp(cbuf,"cat"))     //имя процесса, который МОЖЕТ! работать с файлом
            {
                retvalue = EPERM; // Доступ необходимо заблокировать.
            }
        }
        vnode_putname(vname); // Очистим запрошенное ранее имя нода
    }
    return(retvalue);
}

static int mpo_vnode_check_open
(
    kauth_cred_t cred,
    struct vnode *vp,
    struct label *label,
    int acc_mode
 )
{
    return is_file_accessible(vp);
}

static int mpo_vnode_check_unlink(
    kauth_cred_t cred,
    struct vnode *dvp,
    struct label *dlabel,
    struct vnode *vp,
    struct label *label,
    struct componentname *cnp
 )
{
    return is_file_accessible(vp);
}




kern_return_t kext_2_start(kmod_info_t * ki, void *d);
kern_return_t tkext_2_stop(kmod_info_t *ki, void *d);

kern_return_t kext_2_start(kmod_info_t * ki, void *d)
{
    printf("++++++++++++++++++++++++kext loaded!++++++++++++++++++++++++++++!\n");
    g_policy_ops.mpo_vnode_check_open = (mpo_vnode_check_open_t *)mpo_vnode_check_open;       //открытие файла
    g_policy_ops.mpo_vnode_check_unlink = (mpo_vnode_check_unlink_t *)mpo_vnode_check_unlink; //удаление файла
    
    
    
    g_policy_conf.mpc_name = "MyRandomName";
    g_policy_conf.mpc_fullname = "MyRandomFullName";
    g_policy_conf.mpc_labelnames = NULL;
    g_policy_conf.mpc_labelname_count = 0;
    g_policy_conf.mpc_ops = &g_policy_ops;
    g_policy_conf.mpc_loadtime_flags = MPC_LOADTIME_FLAG_UNLOADOK;
    g_policy_conf.mpc_field_off = NULL;
    g_policy_conf.mpc_runtime_flags = 0;
    g_policy_conf.mpc_list = NULL;
    g_policy_conf.mpc_data = NULL;
    
    mac_policy_register( &g_policy_conf, &g_policy, d );
    
    return KERN_SUCCESS;
}

kern_return_t kext_2_stop(kmod_info_t *ki, void *d)
{
    printf("------------------------kext completed!--------------------------\n");
    mac_policy_unregister(g_policy);
    return KERN_SUCCESS;
}
