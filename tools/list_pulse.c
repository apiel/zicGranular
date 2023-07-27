// gcc -o list_pulse list_pulse.c $(pkg-config --cflags --libs libpulse-simple)
// gcc -o list_pulse list_pulse.c $(pkg-config --cflags --libs libpulse-simple) && ./list_pulse

#include <pulse/pulseaudio.h>
#include <pulse/simple.h>
#include <stdio.h>
#include <string.h>

static void rt_pa_set_server_info(pa_context* context, const pa_server_info* info, void* userdata)
{
    printf("default sink name: %s\n", info->default_sink_name);
}

// Used to get output device information.
static void rt_pa_set_sink_info(pa_context* c, const pa_sink_info* i,
    int eol, void* userdata)
{
    if (eol) {
        pa_mainloop_api* mlApi = (pa_mainloop_api*)userdata;
        mlApi->quit(mlApi, 0);
        return;
    }

    const char* name = pa_proplist_gets(i->proplist, "device.description");
    printf("name: %s (outputChannels %d preferredSampleRate %d)\n", name, i->sample_spec.channels, i->sample_spec.rate);
}

// This is the initial function that is called when the callback is
// set. This one then calls the functions above.
static void rt_pa_context_state_callback(pa_context* context, void* userdata)
{
    pa_mainloop_api* mlApi = (pa_mainloop_api*)userdata;

    switch (pa_context_get_state(context)) {
    case PA_CONTEXT_CONNECTING:
    case PA_CONTEXT_AUTHORIZING:
    case PA_CONTEXT_SETTING_NAME:
        break;

    case PA_CONTEXT_READY:
        printf("PA_CONTEXT_READY\n");
        pa_context_get_server_info(context, rt_pa_set_server_info, userdata); // server info
        pa_context_get_sink_info_list(context, rt_pa_set_sink_info, userdata); // output info ... needs to be before input
        // pa_context_get_source_info_list(context, rt_pa_set_source_info_and_quit, userdata); // input info
        break;

    case PA_CONTEXT_TERMINATED:
        mlApi->quit(mlApi, 0);
        break;

    case PA_CONTEXT_FAILED:
    default:
        printf("PA_CONTEXT_FAILED\n");
        mlApi->quit(mlApi, 0);
    }
}

int main(int argc, char* argv[])
{
    pa_mainloop* ml = NULL;
    pa_context* context = NULL;
    char* server = NULL;
    int ret = 1;
    if (!(ml = pa_mainloop_new())) {
        printf("pa_mainloop_new() failed.\n");
        goto quit;
    }

    pa_mainloop_api* paMainLoopApi = pa_mainloop_get_api(ml);

    if (!(context = pa_context_new_with_proplist(paMainLoopApi, NULL, NULL))) {
        printf("pa_context_new() failed.\n");
        goto quit;
    }

    pa_context_set_state_callback(context, rt_pa_context_state_callback, paMainLoopApi);

    if (pa_context_connect(context, server, PA_CONTEXT_NOFLAGS, NULL) < 0) {
        printf("pa_context_connect() failed: %s\n", pa_strerror(pa_context_errno(context)));
        goto quit;
    }

    if (pa_mainloop_run(ml, &ret) < 0) {
        printf("pa_mainloop_run() failed.\n");
    }

quit:
    if (context)
        pa_context_unref(context);

    if (ml)
        pa_mainloop_free(ml);

    pa_xfree(server);

    return 0;
}
