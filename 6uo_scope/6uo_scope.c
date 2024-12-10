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


#define _6UO_BUF_LEN 56000000
static ViChar vi_response[_6UO_BUF_LEN] = { 0 };

int main()
{
    clock_t time_start;

    ViSession vi_session, vi_scope;
    
#if 0
    ViFindList vi_list;
#endif
    ViUInt32 vi_ret_cnt;
    ViUInt32 vi_free_bytes;

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


    ViChar vi_query[][1024] = { "*IDN?", ":WAV:SOUR CHAN2", ":WAV:MODE RAW", ":WAVeform:POINts 56000000", ":WAV:DATA?", ":WAVeform:PREamble?"};
    vi_ret = viWrite(vi_scope, vi_query[0], (ViUInt32)strlen(vi_query[0]), VI_NULL);
    vi_ret = viRead(vi_scope, vi_response, _6UO_BUF_LEN, &vi_ret_cnt);
    vi_response[vi_ret_cnt] = 0; //terminate the string properly

    _6uo_log_printf("Hello, I am %s", vi_response);

    vi_ret = viWrite(vi_scope, vi_query[1], (ViUInt32)strlen(vi_query[1]), VI_NULL);
    vi_ret |= viWrite(vi_scope, vi_query[2], (ViUInt32)strlen(vi_query[2]), VI_NULL);
    vi_ret |= viWrite(vi_scope, vi_query[3], (ViUInt32)strlen(vi_query[3]), VI_NULL);
    vi_ret |= viWrite(vi_scope, vi_query[4], (ViUInt32)strlen(vi_query[4]), VI_NULL);
    if (vi_ret != VI_SUCCESS)
    {
        _6uo_print_error("viWrite() returned %d. Terminating.\n", vi_ret);
        goto close_scope;
    }

    /**
     * Read the output
     */
    vi_free_bytes = _6UO_BUF_LEN;
    _6uo_log_printf("Start transmission.\n");
    time_start = clock();
    while (1)
    {
        vi_ret = viRead(vi_scope, vi_response, vi_free_bytes, &vi_ret_cnt);
        vi_free_bytes -= vi_ret_cnt;

        if (vi_ret == VI_SUCCESS)
        {
            _6uo_log_printf("Succesfully read %d points.\n", _6UO_BUF_LEN - vi_free_bytes);
            break;
        }
        if (vi_ret != VI_SUCCESS_MAX_CNT)
        {
            _6uo_print_error("viRead() returned %d. Terminating.\n", vi_ret);
            goto close_scope;
        }

#if 0
        if (vi_ret == VI_SUCCESS || vi_ret == VI_SUCCESS_MAX_CNT)
        {
            for (int i = 0; i < vi_ret_cnt; i++)
            {
                fprintf(f, "%d\n", vi_response[i]);
            }
            if (vi_ret == VI_SUCCESS)
            {
                break;
            }
        }
        else
        {
            _6uo_print_error("viRead() returned %d. Terminating.\n", vi_ret);
            fclose(f);
            goto close_scope;
        }
#endif /* 0 */
    }

    _6uo_log_printf("Transmission took %-32d seconds\n", (clock() - time_start) / CLOCKS_PER_SEC);

    /**
     * Write to file
     */
    FILE* f = fopen("test.csv", "w");
    if (!f)
    {
        _6uo_print_error("fopen() error. Terminating.\n");
        goto close_scope;
    }
    fprintf(f, "v\n");

    time_start = clock();
    for (int i = 0; i < _6UO_BUF_LEN - vi_free_bytes; i++)
    {
        fprintf(f, "%d\n", vi_response[i]);
    }
    _6uo_log_printf("Disk write took %-32d seconds\n", (clock() - time_start) / CLOCKS_PER_SEC);

    fclose(f);

    /**
     * Get preamble
     */
    vi_ret = viWrite(vi_scope, vi_query[5], (ViUInt32)strlen(vi_query[5]), VI_NULL);
    if (vi_ret != VI_SUCCESS)
    {
        _6uo_print_error("viWrite() returned %d. Terminating.\n", vi_ret);
        goto close_scope;
    }
    vi_ret = viRead(vi_scope, vi_response, _6UO_BUF_LEN, &vi_ret_cnt);
    if (vi_ret != VI_SUCCESS)
    {
        _6uo_print_error("viRead() returned %d. Terminating.\n", vi_ret);
        goto close_scope;
    }
    vi_response[vi_ret_cnt] = 0; //terminate the string properly
    _6uo_log_printf("Preamble: %s\n", vi_response);
    f = fopen("test_preamble.csv", "w");
    if (!f)
    {
        _6uo_print_error("fopen() error. Terminating.\n");
        goto close_scope;
    }
    fprintf(f,
        "<format>,<type>,<points>,<count>,<xincrement>,<xorigin>,<xreference>,<yincrement>,<yorigin>,<yreference>\n%s\n",
        vi_response);
    fclose(f);


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
