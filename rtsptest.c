#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
static gboolean timeout (GstRTSPServer * server, gboolean ignored)
{
  GstRTSPSessionPool *pool;
 
  pool = gst_rtsp_server_get_session_pool (server);
  gst_rtsp_session_pool_cleanup (pool);
  g_object_unref (pool);
 
  return TRUE;
}
 
int main (int argc, char *argv[])
{
  GMainLoop *loop;
  GstRTSPServer *server;
  GstRTSPMediaMapping *mapping;
  GstRTSPMediaFactory *factory;
 
  gst_init (&argc, &argv);
 
  loop = g_main_loop_new (NULL, FALSE);
 
  /* create a server instance */
  server = gst_rtsp_server_new ();
 
  /* get the mapping for this server, every server has a default mapper object
   * that be used to map uri mount points to media factories */
  mapping = gst_rtsp_server_get_media_mapping (server);
 
 
  /* make a media factory for a test stream. The default media factory can use
   * gst-launch syntax to create pipelines.
   * any launch line works as long as it contains elements named pay%d. Each
   * element with pay%d names will be a stream */
  factory = gst_rtsp_media_factory_new ();
 
  gst_rtsp_media_factory_set_launch (factory, "( "
      "filesrc location=2-1.mp3 ! mad ! audioconvert !  rtpL16pay name=pay0 pt=96 " ")");
  /* attach the test factory to the /test url */
  gst_rtsp_media_mapping_add_factory (mapping, "/test", factory);
 
  /* don't need the ref to the mapper anymore */
  g_object_unref (mapping);
 
  /* attach the server to the default maincontext */
  if (gst_rtsp_server_attach (server, NULL) == 0)
    goto failed;
 
  /* add a timeout for the session cleanup */
  g_timeout_add_seconds (2, (GSourceFunc) timeout, server);
 
  /* start serving, this never stops */
  g_main_loop_run (loop);
 
  return 0;
 
  /* ERRORS */
failed:
  {
    g_print ("failed to attach the server\n");
    return -1;
  }
}
