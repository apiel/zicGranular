// gcc -o test5 test5.c $(pkg-config --cflags --libs libpulse-simple)

#include <pulse/pulseaudio.h>
#include <pulse/simple.h>
#include <stdio.h>
#include <string.h>

// // A structure needed to pass variables for device probing.
// struct PaDeviceProbeInfo {
//     pa_mainloop_api* paMainLoopApi;
//     //   std::string defaultSinkName;
//     //   std::string defaultSourceName;
//     //   int defaultRate;
//     //   unsigned int *currentDeviceId;
//     //   std::vector< std::string > deviceNames;
//     //   std::vector< RtApiPulse::PaDeviceInfo > *paDeviceList;
//     //   std::vector< RtAudio::DeviceInfo > *rtDeviceList;
// };

// The following 3 functions are called by the device probing
// system. This first one gets overall system information.
static void rt_pa_set_server_info(pa_context* context, const pa_server_info* info, void* userdata)
{
    (void)context;
    pa_sample_spec ss;

    //   PaDeviceProbeInfo *paProbeInfo = static_cast<PaDeviceProbeInfo *>( userdata );
    //   if (!info) {
    //     paProbeInfo->paMainLoopApi->quit( paProbeInfo->paMainLoopApi, 1 );
    //     return;
    //   }

    //   ss = info->sample_spec;
    //   paProbeInfo->defaultRate = ss.rate;
    //   paProbeInfo->defaultSinkName = info->default_sink_name;
    //   paProbeInfo->defaultSourceName = info->default_source_name;
    printf("default sink name: %s\n", info->default_sink_name);
}

// Used to get output device information.
static void rt_pa_set_sink_info(pa_context* c, const pa_sink_info* i,
    int eol, void* userdata)
{
    if (eol)
        return;

    //   PaDeviceProbeInfo *paProbeInfo = static_cast<PaDeviceProbeInfo *>( userdata );
    char* name = pa_proplist_gets(i->proplist, "device.description");
    //   paProbeInfo->deviceNames.push_back( name );
    //   for ( size_t n=0; n<paProbeInfo->rtDeviceList->size(); n++ )
    //     if ( paProbeInfo->rtDeviceList->at(n).name == name ) return; // we've already probed this one

    //   RtAudio::DeviceInfo info;
    //   info.name = name;
    //   info.outputChannels = i->sample_spec.channels;
    //   info.preferredSampleRate = i->sample_spec.rate;
    //   info.isDefaultOutput = ( paProbeInfo->defaultSinkName == i->name );
    //   for ( const unsigned int *sr = SUPPORTED_SAMPLERATES; *sr; ++sr )
    //     info.sampleRates.push_back( *sr );
    //   for ( const rtaudio_pa_format_mapping_t *fm = supported_sampleformats; fm->rtaudio_format; ++fm )
    //     info.nativeFormats |= fm->rtaudio_format;
    //   info.ID = *(paProbeInfo->currentDeviceId);
    //   *(paProbeInfo->currentDeviceId) = info.ID + 1;
    //   paProbeInfo->rtDeviceList->push_back( info );

    //   RtApiPulse::PaDeviceInfo painfo;
    //   painfo.sinkName = i->name;
    //   paProbeInfo->paDeviceList->push_back( painfo );
    printf("name: %s (outputChannels %d preferredSampleRate %d)\n", name, i->sample_spec.channels, i->sample_spec.rate);
}

// This is the initial function that is called when the callback is
// set. This one then calls the functions above.
static void rt_pa_context_state_callback(pa_context* context, void* userdata)
{
    //   PaDeviceProbeInfo *paProbeInfo = static_cast<PaDeviceProbeInfo *>( userdata );
    auto state = pa_context_get_state(context);
    switch (state) {
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
        printf("PA_CONTEXT_TERMINATED\n");
        //   paProbeInfo->paMainLoopApi->quit( paProbeInfo->paMainLoopApi, 0 );
        break;

    case PA_CONTEXT_FAILED:
    default:
        printf("PA_CONTEXT_FAILED\n");
        //   paProbeInfo->paMainLoopApi->quit( paProbeInfo->paMainLoopApi, 1 );
    }
}

int main(int argc, char* argv[])
{
    pa_simple* s;
    pa_sample_spec ss;

    ss.format = PA_SAMPLE_S16NE;
    ss.channels = 2;
    ss.rate = 44100;

    s = pa_simple_new(
        NULL, // Use the default server.
        "Fooapp", // Our application's name.
        PA_STREAM_PLAYBACK,
        NULL, // Use the default device.
        "Music", // Description of our stream.
        &ss, // Our sample format.
        NULL, // Use default channel map
        NULL, // Use default buffering attributes.
        NULL // Ignore error code.
    );

    pa_simple_free(s);

    // pa_mainloop* ml = NULL;
    // if (!(ml = pa_mainloop_new())) {
    //     fprintf(stderr, "pa_mainloop_new() failed.\n");
    //     return 1;
    // }

    // pa_mainloop_free(ml);

    pa_mainloop* ml = NULL;
    pa_context* context = NULL;
    char* server = NULL;
    int ret = 1;
    if (!(ml = pa_mainloop_new())) {
        printf("pa_mainloop_new() failed.\n");
        goto quit;
    }

    pa_mainloop_api* paMainLoopApi = pa_mainloop_get_api(ml);
    // unsigned int * currentDeviceId = &currentDeviceId_;
    // paProbeInfo.paDeviceList = &paDeviceList_;
    // paProbeInfo.rtDeviceList = &deviceList_;

    if (!(context = pa_context_new_with_proplist(paMainLoopApi, NULL, NULL))) {
        printf("pa_context_new() failed.\n");
        goto quit;
    }

    pa_context_set_state_callback(context, rt_pa_context_state_callback, NULL);

    if (pa_context_connect(context, server, PA_CONTEXT_NOFLAGS, NULL) < 0) {
        printf("pa_context_connect() failed: %s\n", pa_strerror(pa_context_errno(context)));
        goto quit;
    }

    if (pa_mainloop_run(ml, &ret) < 0) {
        printf("pa_mainloop_run() failed.\n");
        goto quit;
    }

    if (ret != 0) {
        printf("pa_mainloop_run() returned %d.\n", ret);
        goto quit;
    }

    // // Check for devices that have been unplugged.
    // unsigned int m;
    // for (std::vector<RtAudio::DeviceInfo>::iterator it = deviceList_.begin(); it != deviceList_.end();) {
    //     for (m = 0; m < paProbeInfo.deviceNames.size(); m++) {
    //         if ((*it).name == paProbeInfo.deviceNames[m]) {
    //             ++it;
    //             break;
    //         }
    //     }
    //     if (m == paProbeInfo.deviceNames.size()) { // not found so remove it from our list
    //         it = deviceList_.erase(it);
    //         paDeviceList_.erase(paDeviceList_.begin() + distance(deviceList_.begin(), it));
    //     }
    // }

quit:
    if (context)
        pa_context_unref(context);

    if (ml)
        pa_mainloop_free(ml);

    pa_xfree(server);

    return 0;
}
