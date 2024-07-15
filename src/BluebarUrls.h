
// TODO: Fiddler interferes HTTPS traffic when trying to access local
//		 server with self-signed SSL certificate.
// For testing purposes only (until the above issue is solved).
#define FORCE_HTTP


namespace BluebarUrls
{

#if defined _DEBUG && defined FORCE_HTTP

const char kHomeUri[] = "http://www.thebluebar.com/desktop-apps/";
const char kManifestUri[] = "http://www.thebluebar.com/download/download-manifest.json";
const char kEventLoggerUri[] = "http://www.thebluebar.com/ajax/event_log.php";

#else

const char kHomeUri[] = "https://www.thebluebar.com/desktop-apps/";
const char kManifestUri[] = "https://www.thebluebar.com/download/download-manifest.json";
const char kEventLoggerUri[] = "https://www.thebluebar.com/ajax/event_log.php";

#endif // FORCE_HTTP

} // BluebarUrls