#ifndef __COMMUNICATE_H__
#define __COMMUNICATE_H__


#define COMM_TASK_PERIOD        10 //ms
#define IP                      "192.168.0.29"
#define PORT                    "5762"  

#ifdef __cplusplus
extern "C" {
#endif
extern void commSetUp(void);
#ifdef __cplusplus
}
#endif

#endif /* __COMMUNICATE_H__ */
