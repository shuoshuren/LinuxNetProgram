#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int child_events = 0;

/*信号处理函数*/
void child_sig_handler(int x){
    child_events++;
    signal(SIGCHLD,child_sig_handler);
}

int main(void){
    /*设定信号掩码sigmask和原始信号掩码orig_sigmask*/
    sigset_t sigmask,orig_sigmask;
    sigemptyset(&sigmask);//清空信号
    sigaddset(&sigmask,SIGCHLD);//将信号加入sigmask

    //设定信号sig_block掩码sigmask并将原始掩码保存到orig_sigmask中
    sigprocmask(SIG_BLOCK,&sigmask,&orig_sigmask);
    signal(SIGCHLD,child_sig_handler());
    
    for(;;){
        for(;child_events>0;child_events--){
        
        }

        r = pselect(nfds,&rd,&wr,&er,0,&orig_sigmask);
    }

    return 0;
}
