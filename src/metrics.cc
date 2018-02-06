////////////////////////////////////////////////////////////////////////
// ./src/metrics.cc.cpp
// generated with ecppc
//

#include <tnt/ecpp.h>
#include <tnt/convert.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/httpheader.h>
#include <tnt/http.h>
#include <tnt/data.h>
#include <tnt/componentfactory.h>
#include <cxxtools/log.h>
#include <stdexcept>

log_define("component.metrics")

// <%pre>
#line 5 "./src/metrics.ecpp"

#include "tntmlm.h"
#include <fty_proto.h>

//out <label>{device="<device-id>"} <value>
static
void s_rt_dump_metrics(const char*label, const char* name, const char*filter,MlmClientPool::Ptr &client,tnt::HttpReply& reply)
{
    printf("s_rt_dump_metrics %s %s %s",label,name,filter);
    static const char* method = "GET";
    zmsg_t *msg = zmsg_new ();
    zuuid_t *uuid = zuuid_new ();
    zmsg_addstr (msg, zuuid_str_canonical (uuid));
    zmsg_addstr (msg, method);
    zmsg_addstr (msg, name);
    if(filter!=NULL)
        zmsg_addstr (msg, filter);
    
    int rv = client->sendto ("fty-metric-cache", "latest-rt-data", 1000, &msg);
    if (rv == -1) {
        zuuid_destroy (&uuid);
        printf (
                "client->sendto (address = '%s', subject = '%s', timeout = 1000) failed.",
                "fty-metric-cache", "latest-rt-data");
        return ;
    }

    msg = client->recv (zuuid_str_canonical (uuid), 5);
    zuuid_destroy (&uuid);
    if (!msg) {
        printf ("client->recv (timeout = '5') returned NULL");
        return ;
    }

    char *result = zmsg_popstr (msg);
    if (NULL==result || !streq (result, "OK")) {
        printf ("Error reply for device '%s', result=%s", name, (result==NULL)?"NULL":result);
        if(result!=NULL)
            zstr_free (&result);
        zmsg_destroy (&msg);
        return ;
    }
    char *element = zmsg_popstr (msg);
    if (!streq (element, name)) {
        printf ("element name (%s) from message differs from requested one (%s), ignoring", element,name);
        zstr_free (&element);
        zmsg_destroy (&msg);
        return ;
    }
    zstr_free (&element);

    if(msg==NULL)
        return;
    zmsg_t *data = zmsg_popmsg (msg);
    while (data) {
        fty_proto_t *bmsg = fty_proto_decode (&data);
        if (!bmsg) {
            printf ("decoding fty_proto_t failed");
            zmsg_destroy (&msg);
            return;
        }

        // TODO: non double values are not (yet) supported
        double dvalue = 0.0;
        try {
            dvalue = std::stod (fty_proto_value (bmsg));
        } catch (const std::exception& e) {
            printf ("fty_proto_value () returned a string that does not encode a double value: '%s'. Defaulting to 0.0 value.", fty_proto_value (bmsg));
        }
        reply.out() << label;
        reply.out() << "{ device=\"";
        reply.out() << fty_proto_name(bmsg);
        reply.out() << "\"} ";
        reply.out() << dvalue;
        reply.out() << "\n";
        fty_proto_destroy (&bmsg);
        data = zmsg_popmsg (msg);
    }
    zmsg_destroy (&msg);
}

// </%pre>

namespace
{
class _component_ : public tnt::EcppComponent
{
    _component_& main()  { return *this; }

  protected:
    ~_component_();

  public:
    _component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl);

    unsigned operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam);
};

static tnt::ComponentFactoryImpl<_component_> Factory("metrics");

static const char* rawData = "\030\000\000\000\031\000\000\000f\000\000\000\277\000\000\000\030\001\000\000\031\001\000"
  "\000\n# HELP ipm_realpower_watt current realpower \n# TYPE ipm_realpower_watt gauge\n# HELP ipm_ambiant_celcius curre"
  "nt ambiant temperature \n# TYPE ipm_ambiant_celcius gauge\n# HELP ipm_ambiant_celcius current ambiant temperature \n#"
  " TYPE ipm_ambiant_celcius gauge\n\n";

// <%shared>
// </%shared>

// <%config>
// </%config>

#define SET_LANG(lang) \
     do \
     { \
       request.setLang(lang); \
       reply.setLocale(request.getLocale()); \
     } while (false)

_component_::_component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl)
  : EcppComponent(ci, um, cl)
{
  // <%init>
  // </%init>
}

_component_::~_component_()
{
  // <%cleanup>
  // </%cleanup>
}

unsigned _component_::operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam)
{
  log_trace("metrics " << qparam.getUrl());

  tnt::DataChunks data(rawData);

  // <%cpp>
  reply.out() << data[0]; // \n
#line 85 "./src/metrics.ecpp"

    // connect to malamute
    MlmClientPool::Ptr client = mlm_pool.get ();
    if (!client.getPointer ()) {
        printf ("mlm_pool.get () failed.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

  reply.out() << data[1]; // # HELP ipm_realpower_watt current realpower \n# TYPE ipm_realpower_watt gauge\n
#line 95 "./src/metrics.ecpp"

   s_rt_dump_metrics("ipm_realpower_watt",".*","realpower.default$",client,reply);

  reply.out() << data[2]; // # HELP ipm_ambiant_celcius current ambiant temperature \n# TYPE ipm_ambiant_celcius gauge\n
#line 100 "./src/metrics.ecpp"

   s_rt_dump_metrics("ipm_ambiant_celcius",".*","temperatur$",client,reply);

  reply.out() << data[3]; // # HELP ipm_ambiant_celcius current ambiant temperature \n# TYPE ipm_ambiant_celcius gauge\n
#line 105 "./src/metrics.ecpp"

   s_rt_dump_metrics("ipm_ambiant_percent",".*","humidity$",client,reply);

  reply.out() << data[4]; // \n
  // <%/cpp>
  return HTTP_OK;
}

} // namespace
