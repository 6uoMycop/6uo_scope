#include "6uo_scope.h"


static volatile uint8_t stop = 0;


/**
 * SIGINT handler.
 */
static void sigint_handler(int sig)
{
    (void)sig;

    stop = 1;
}


void worker_loop()
{
    while (!stop)
    {
        ;
    }
}


#define _6UO_BUF_LEN 1024

int main()
{
    _6uo_log_printf("Hello CMake.\n");
    
    ViSession vi_session, vi_scope;
    
#if 0
    ViFindList vi_list;
#endif
    ViChar vi_response[_6UO_BUF_LEN] = {0};
    ViUInt32 vi_ret_cnt;

    ViStatus vi_ret = VI_SUCCESS;

    /**
     * SIGINT handler.
     */
    signal(SIGINT, sigint_handler);

    vi_ret = viOpenDefaultRM(&vi_session);
    if (vi_ret != VI_SUCCESS)
    {
        _6uo_print_error("viOpenDefaultRM() returned %d. Terminating.\n", vi_ret);
        goto terminate;
    }

#if 0
    while (1)
    {
        if (stop)
        {
            goto close_session;
        }

        vi_ret = viFindRsrc(vi_session,
            //"TCPIP?*INSTR",
            "?*",
            &vi_list, &vi_list_cnt, vi_desc);

        if (vi_ret == VI_SUCCESS)
        {
            break;
        }
        if (vi_ret != VI_ERROR_RSRC_NFOUND)
        {
            _6uo_print_error("viFindRsrc() returned %d. Terminating.\n", vi_ret);
            goto close_session;
        }
        Sleep(1000);
    }
#endif

    vi_ret = viOpen(vi_session, "TCPIP::192.168.137.2::INSTR", VI_NULL, VI_NULL, &vi_scope);
    if (vi_ret != VI_SUCCESS)
    {
        _6uo_print_error("viOpen() returned %d. Terminating.\n", vi_ret);
        goto close_session;
    }
    vi_ret = viClear(vi_scope); // clear instrument's io buffers
    if (vi_ret != VI_SUCCESS)
    {
        _6uo_print_error("viOpen() returned %d. Terminating.\n", vi_ret);
        goto close_scope;
    }

    ViChar vi_query[] = "*IDN?";
    vi_ret = viWrite(vi_scope, vi_query, (ViUInt32)strlen(vi_query), VI_NULL);
    vi_ret = viRead(vi_scope, vi_response, _6UO_BUF_LEN, &vi_ret_cnt);
    vi_response[vi_ret_cnt] = 0; //terminate the string properly

    _6uo_log_printf("Hello, I am %s", vi_response);


close_scope:
    vi_ret = viClose(vi_scope);
    if (vi_ret != VI_SUCCESS)
    {
        _6uo_print_error("Warn: viClose() returned %d.\n", vi_ret);
    }

#if 0
close_list:
    vi_ret = viClose(vi_list);
    if (vi_ret != VI_SUCCESS)
    {
        _6uo_print_error("Warn: viClose() returned %d.\n", vi_ret);
    }
#endif

close_session:
    vi_ret = viClose(vi_session);
    if (vi_ret != VI_SUCCESS)
    {
        _6uo_print_error("Warn: viClose() returned %d.\n", vi_ret);
    }

terminate:
    _6uo_log_printf("Exiting (%d)\n", vi_ret);
    return vi_ret;
}
