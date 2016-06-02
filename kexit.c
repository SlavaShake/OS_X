//
//  kekxit.c
//  kekxit
//
//  Created by Admin on 31.05.16.
//  Copyright (c) 2016 Admin. All rights reserved.
//

#include <mach/mach_types.h>
#include <sys/kernel_types.h>
#include <sys/types.h>
struct componentname;
#include <security/mac_policy.h>
#include <sys/param.h>
#include <sys/vnode.h>
#include <sys/proc.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach/mach_types.h>




kern_return_t kekxit_start(kmod_info_t * ki, void *d);
kern_return_t kekxit_stop(kmod_info_t *ki, void *d);

static mpo_vnode_check_unlink_t mac_policy_unlink;
static mac_policy_handle_t mac_handle;

static struct mac_policy_ops mac_ops =
{
    .mpo_vnode_check_unlink = mac_policy_unlink  // обработчик удаления файла
};

// А также структуру, содержащую информацию о нашей политике.
static struct mac_policy_conf mac_policy_conf =
{
    .mpc_name               =   "DONT_DELL_FILE",
    .mpc_fullname           =   "DONT_DELL_FILE_FULL",
    .mpc_labelnames         =   NULL,
    .mpc_labelname_count    =   0,
    .mpc_ops                =   &mac_ops,
    .mpc_loadtime_flags     =   MPC_LOADTIME_FLAG_UNLOADOK, // Политика ВЫГРУЖАЕМА!
    .mpc_field_off          =   NULL,
    .mpc_runtime_flags      =   0
};


static int is_file_accessible(struct vnode *vp)
{
    const char *vname = NULL;
    char cbuf[MAXCOMLEN+1];
    int retvalue = 0;
    
    if (vp == NULL) // В ряде случаев отсутствие информации о ноде нормально,
        // поэтому разрешаем продолжить выполнение функции.
        return (retvalue);
    vname = vnode_getname(vp);
    if(vname) // Имя нода не пустое
    {
        
        // И идет попытка получить
        // доступ к  файлу
        if(
            strcasecmp(vname, "file_my_log.log")    == 0 ||
            strcasecmp(vname, "my_log.log")         == 0 ||
            strcasecmp(vname, "file_y_log.log")     == 0 ||
            strcasecmp(vname, "file_m_log.log")     == 0
           )
        {
            proc_selfname(cbuf, sizeof(cbuf));
            // и процесс обращающийся к файлу не является
            // системой обновления антивируса
            if (strcasecmp(cbuf,"cat"))
            {
                retvalue = EPERM; // Доступ необходимо заблокировать.
            }
        }
        
        vnode_putname(vname); // Очистим запрошенное ранее имя нода
    }
    return(retvalue);
}


static int mac_policy_unlink(
                             kauth_cred_t cred,
                             struct vnode *dvp,
                             struct label *dlabel,
                             struct vnode *vp,
                             struct label *label,
                             struct componentname *cnp
                             )
{
    printf("Get call mac_policy_UNLINK!\n");
    return is_file_accessible(vp);
}


kern_return_t kekxit_start(kmod_info_t * ki, void *d)
{
    // Заполним структуру указателями на функции обратного вызова.
    // В случае успешного выполнения функции mac_policy_register
    // в переменную mac_handle будет записан указатель
    // на зарегистрированную политику.
    // Зарегистрируем политику.
    printf("Start kern_return_t kekxit_START!\n");
    
    return mac_policy_register(&mac_policy_conf, &mac_handle, d);
}




kern_return_t kekxit_stop(kmod_info_t *ki, void *d)
{
    printf("Start kern_return_t kekxit_STOP!\n");
    return mac_policy_unregister(mac_handle);
    // Deregistering the policy
}
