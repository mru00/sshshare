// Copyright (C) 2005-2010 Code Synthesis Tools CC
//
// This program was generated by CodeSynthesis XSD, an XML Schema to
// C++ data binding compiler.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
// In addition, as a special exception, Code Synthesis Tools CC gives
// permission to link this program with the Xerces-C++ library (or with
// modified versions of Xerces-C++ that use the same license as Xerces-C++),
// and distribute linked combinations including the two. You must obey
// the GNU General Public License version 2 in all respects for all of
// the code used other than Xerces-C++. If you modify this copy of the
// program, you may extend this exception to your version of the program,
// but you are not obligated to do so. If you do not wish to do so, delete
// this exception statement from your version.
//
// Furthermore, Code Synthesis Tools CC makes a special exception for
// the Free/Libre and Open Source Software (FLOSS) which is described
// in the accompanying FLOSSE file.
//

// Begin prologue.
//
//
// End prologue.

#include <xsd/cxx/pre.hxx>

#include "sshshare_config.hxx"

// config_t
// 

const config_t::username_type& config_t::
username () const
{
  return this->username_.get ();
}

config_t::username_type& config_t::
username ()
{
  return this->username_.get ();
}

void config_t::
username (const username_type& x)
{
  this->username_.set (x);
}

void config_t::
username (::std::auto_ptr< username_type > x)
{
  this->username_.set (x);
}

const config_t::username_type& config_t::
username_default_value ()
{
  return username_default_value_;
}

const config_t::server_type& config_t::
server () const
{
  return this->server_.get ();
}

config_t::server_type& config_t::
server ()
{
  return this->server_.get ();
}

void config_t::
server (const server_type& x)
{
  this->server_.set (x);
}

void config_t::
server (::std::auto_ptr< server_type > x)
{
  this->server_.set (x);
}

const config_t::server_type& config_t::
server_default_value ()
{
  return server_default_value_;
}


#include <xsd/cxx/xml/dom/parsing-source.hxx>

// config_t
//

const config_t::username_type config_t::username_default_value_ (
  "user");

const config_t::server_type config_t::server_default_value_ (
  "www.example.com");

config_t::
config_t (const username_type& username,
          const server_type& server)
: ::xml_schema::type (),
  username_ (username, ::xml_schema::flags (), this),
  server_ (server, ::xml_schema::flags (), this)
{
}

config_t::
config_t (const config_t& x,
          ::xml_schema::flags f,
          ::xml_schema::container* c)
: ::xml_schema::type (x, f, c),
  username_ (x.username_, f, this),
  server_ (x.server_, f, this)
{
}

config_t::
config_t (const ::xercesc::DOMElement& e,
          ::xml_schema::flags f,
          ::xml_schema::container* c)
: ::xml_schema::type (e, f | ::xml_schema::flags::base, c),
  username_ (f, this),
  server_ (f, this)
{
  if ((f & ::xml_schema::flags::base) == 0)
  {
    ::xsd::cxx::xml::dom::parser< char > p (e, true, false);
    this->parse (p, f);
  }
}

void config_t::
parse (::xsd::cxx::xml::dom::parser< char >& p,
       ::xml_schema::flags f)
{
  for (; p.more_elements (); p.next_element ())
  {
    const ::xercesc::DOMElement& i (p.cur_element ());
    const ::xsd::cxx::xml::qualified_name< char > n (
      ::xsd::cxx::xml::dom::name< char > (i));

    // username
    //
    if (n.name () == "username" && n.namespace_ ().empty ())
    {
      ::std::auto_ptr< username_type > r (
        username_traits::create (i, f, this));

      if (!username_.present ())
      {
        this->username_.set (r);
        continue;
      }
    }

    // server
    //
    if (n.name () == "server" && n.namespace_ ().empty ())
    {
      ::std::auto_ptr< server_type > r (
        server_traits::create (i, f, this));

      if (!server_.present ())
      {
        this->server_.set (r);
        continue;
      }
    }

    break;
  }

  if (!username_.present ())
  {
    throw ::xsd::cxx::tree::expected_element< char > (
      "username",
      "");
  }

  if (!server_.present ())
  {
    throw ::xsd::cxx::tree::expected_element< char > (
      "server",
      "");
  }
}

config_t* config_t::
_clone (::xml_schema::flags f,
        ::xml_schema::container* c) const
{
  return new class config_t (*this, f, c);
}

config_t::
~config_t ()
{
}

#include <istream>
#include <xsd/cxx/xml/sax/std-input-source.hxx>
#include <xsd/cxx/tree/error-handler.hxx>

::std::auto_ptr< ::config_t >
configuration (const ::std::string& u,
               ::xml_schema::flags f,
               const ::xml_schema::properties& p)
{
  ::xsd::cxx::xml::auto_initializer i (
    (f & ::xml_schema::flags::dont_initialize) == 0,
    (f & ::xml_schema::flags::keep_dom) == 0);

  ::xsd::cxx::tree::error_handler< char > h;

  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::xsd::cxx::xml::dom::parse< char > (
      u, h, p, f));

  h.throw_if_failed< ::xsd::cxx::tree::parsing< char > > ();

  ::std::auto_ptr< ::config_t > r (
    ::configuration (
      d, f | ::xml_schema::flags::own_dom, p));

  return r;
}

::std::auto_ptr< ::config_t >
configuration (const ::std::string& u,
               ::xml_schema::error_handler& h,
               ::xml_schema::flags f,
               const ::xml_schema::properties& p)
{
  ::xsd::cxx::xml::auto_initializer i (
    (f & ::xml_schema::flags::dont_initialize) == 0,
    (f & ::xml_schema::flags::keep_dom) == 0);

  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::xsd::cxx::xml::dom::parse< char > (
      u, h, p, f));

  if (!d.get ())
    throw ::xsd::cxx::tree::parsing< char > ();

  ::std::auto_ptr< ::config_t > r (
    ::configuration (
      d, f | ::xml_schema::flags::own_dom, p));

  return r;
}

::std::auto_ptr< ::config_t >
configuration (const ::std::string& u,
               ::xercesc::DOMErrorHandler& h,
               ::xml_schema::flags f,
               const ::xml_schema::properties& p)
{
  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::xsd::cxx::xml::dom::parse< char > (
      u, h, p, f));

  if (!d.get ())
    throw ::xsd::cxx::tree::parsing< char > ();

  ::std::auto_ptr< ::config_t > r (
    ::configuration (
      d, f | ::xml_schema::flags::own_dom, p));

  return r;
}

::std::auto_ptr< ::config_t >
configuration (::std::istream& is,
               ::xml_schema::flags f,
               const ::xml_schema::properties& p)
{
  ::xsd::cxx::xml::auto_initializer i (
    (f & ::xml_schema::flags::dont_initialize) == 0,
    (f & ::xml_schema::flags::keep_dom) == 0);

  ::xsd::cxx::xml::sax::std_input_source isrc (is);
  return ::configuration (isrc, f, p);
}

::std::auto_ptr< ::config_t >
configuration (::std::istream& is,
               ::xml_schema::error_handler& h,
               ::xml_schema::flags f,
               const ::xml_schema::properties& p)
{
  ::xsd::cxx::xml::auto_initializer i (
    (f & ::xml_schema::flags::dont_initialize) == 0,
    (f & ::xml_schema::flags::keep_dom) == 0);

  ::xsd::cxx::xml::sax::std_input_source isrc (is);
  return ::configuration (isrc, h, f, p);
}

::std::auto_ptr< ::config_t >
configuration (::std::istream& is,
               ::xercesc::DOMErrorHandler& h,
               ::xml_schema::flags f,
               const ::xml_schema::properties& p)
{
  ::xsd::cxx::xml::sax::std_input_source isrc (is);
  return ::configuration (isrc, h, f, p);
}

::std::auto_ptr< ::config_t >
configuration (::std::istream& is,
               const ::std::string& sid,
               ::xml_schema::flags f,
               const ::xml_schema::properties& p)
{
  ::xsd::cxx::xml::auto_initializer i (
    (f & ::xml_schema::flags::dont_initialize) == 0,
    (f & ::xml_schema::flags::keep_dom) == 0);

  ::xsd::cxx::xml::sax::std_input_source isrc (is, sid);
  return ::configuration (isrc, f, p);
}

::std::auto_ptr< ::config_t >
configuration (::std::istream& is,
               const ::std::string& sid,
               ::xml_schema::error_handler& h,
               ::xml_schema::flags f,
               const ::xml_schema::properties& p)
{
  ::xsd::cxx::xml::auto_initializer i (
    (f & ::xml_schema::flags::dont_initialize) == 0,
    (f & ::xml_schema::flags::keep_dom) == 0);

  ::xsd::cxx::xml::sax::std_input_source isrc (is, sid);
  return ::configuration (isrc, h, f, p);
}

::std::auto_ptr< ::config_t >
configuration (::std::istream& is,
               const ::std::string& sid,
               ::xercesc::DOMErrorHandler& h,
               ::xml_schema::flags f,
               const ::xml_schema::properties& p)
{
  ::xsd::cxx::xml::sax::std_input_source isrc (is, sid);
  return ::configuration (isrc, h, f, p);
}

::std::auto_ptr< ::config_t >
configuration (::xercesc::InputSource& i,
               ::xml_schema::flags f,
               const ::xml_schema::properties& p)
{
  ::xsd::cxx::tree::error_handler< char > h;

  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::xsd::cxx::xml::dom::parse< char > (
      i, h, p, f));

  h.throw_if_failed< ::xsd::cxx::tree::parsing< char > > ();

  ::std::auto_ptr< ::config_t > r (
    ::configuration (
      d, f | ::xml_schema::flags::own_dom, p));

  return r;
}

::std::auto_ptr< ::config_t >
configuration (::xercesc::InputSource& i,
               ::xml_schema::error_handler& h,
               ::xml_schema::flags f,
               const ::xml_schema::properties& p)
{
  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::xsd::cxx::xml::dom::parse< char > (
      i, h, p, f));

  if (!d.get ())
    throw ::xsd::cxx::tree::parsing< char > ();

  ::std::auto_ptr< ::config_t > r (
    ::configuration (
      d, f | ::xml_schema::flags::own_dom, p));

  return r;
}

::std::auto_ptr< ::config_t >
configuration (::xercesc::InputSource& i,
               ::xercesc::DOMErrorHandler& h,
               ::xml_schema::flags f,
               const ::xml_schema::properties& p)
{
  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::xsd::cxx::xml::dom::parse< char > (
      i, h, p, f));

  if (!d.get ())
    throw ::xsd::cxx::tree::parsing< char > ();

  ::std::auto_ptr< ::config_t > r (
    ::configuration (
      d, f | ::xml_schema::flags::own_dom, p));

  return r;
}

::std::auto_ptr< ::config_t >
configuration (const ::xercesc::DOMDocument& d,
               ::xml_schema::flags f,
               const ::xml_schema::properties& p)
{
  if (f & ::xml_schema::flags::keep_dom)
  {
    ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > c (
      static_cast< ::xercesc::DOMDocument* > (d.cloneNode (true)));

    ::std::auto_ptr< ::config_t > r (
      ::configuration (
        c, f | ::xml_schema::flags::own_dom, p));

    return r;
  }

  const ::xercesc::DOMElement& e (*d.getDocumentElement ());
  const ::xsd::cxx::xml::qualified_name< char > n (
    ::xsd::cxx::xml::dom::name< char > (e));

  if (n.name () == "configuration" &&
      n.namespace_ () == "")
  {
    ::std::auto_ptr< ::config_t > r (
      ::xsd::cxx::tree::traits< ::config_t, char >::create (
        e, f, 0));
    return r;
  }

  throw ::xsd::cxx::tree::unexpected_element < char > (
    n.name (),
    n.namespace_ (),
    "configuration",
    "");
}

::std::auto_ptr< ::config_t >
configuration (::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument >& d,
               ::xml_schema::flags f,
               const ::xml_schema::properties&)
{
  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > c (
    ((f & ::xml_schema::flags::keep_dom) &&
     !(f & ::xml_schema::flags::own_dom))
    ? static_cast< ::xercesc::DOMDocument* > (d->cloneNode (true))
    : 0);

  ::xercesc::DOMDocument& doc (c.get () ? *c : *d);
  const ::xercesc::DOMElement& e (*doc.getDocumentElement ());

  const ::xsd::cxx::xml::qualified_name< char > n (
    ::xsd::cxx::xml::dom::name< char > (e));

  if (f & ::xml_schema::flags::keep_dom)
    doc.setUserData (::xml_schema::dom::tree_node_key,
                     (c.get () ? &c : &d),
                     0);

  if (n.name () == "configuration" &&
      n.namespace_ () == "")
  {
    ::std::auto_ptr< ::config_t > r (
      ::xsd::cxx::tree::traits< ::config_t, char >::create (
        e, f, 0));
    return r;
  }

  throw ::xsd::cxx::tree::unexpected_element < char > (
    n.name (),
    n.namespace_ (),
    "configuration",
    "");
}

#include <ostream>
#include <xsd/cxx/tree/error-handler.hxx>
#include <xsd/cxx/xml/dom/serialization-source.hxx>

void
configuration (::std::ostream& o,
               const ::config_t& s,
               const ::xml_schema::namespace_infomap& m,
               const ::std::string& e,
               ::xml_schema::flags f)
{
  ::xsd::cxx::xml::auto_initializer i (
    (f & ::xml_schema::flags::dont_initialize) == 0);

  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::configuration (s, m, f));

  ::xsd::cxx::tree::error_handler< char > h;

  ::xsd::cxx::xml::dom::ostream_format_target t (o);
  if (!::xsd::cxx::xml::dom::serialize (t, *d, e, h, f))
  {
    h.throw_if_failed< ::xsd::cxx::tree::serialization< char > > ();
  }
}

void
configuration (::std::ostream& o,
               const ::config_t& s,
               ::xml_schema::error_handler& h,
               const ::xml_schema::namespace_infomap& m,
               const ::std::string& e,
               ::xml_schema::flags f)
{
  ::xsd::cxx::xml::auto_initializer i (
    (f & ::xml_schema::flags::dont_initialize) == 0);

  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::configuration (s, m, f));
  ::xsd::cxx::xml::dom::ostream_format_target t (o);
  if (!::xsd::cxx::xml::dom::serialize (t, *d, e, h, f))
  {
    throw ::xsd::cxx::tree::serialization< char > ();
  }
}

void
configuration (::std::ostream& o,
               const ::config_t& s,
               ::xercesc::DOMErrorHandler& h,
               const ::xml_schema::namespace_infomap& m,
               const ::std::string& e,
               ::xml_schema::flags f)
{
  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::configuration (s, m, f));
  ::xsd::cxx::xml::dom::ostream_format_target t (o);
  if (!::xsd::cxx::xml::dom::serialize (t, *d, e, h, f))
  {
    throw ::xsd::cxx::tree::serialization< char > ();
  }
}

void
configuration (::xercesc::XMLFormatTarget& t,
               const ::config_t& s,
               const ::xml_schema::namespace_infomap& m,
               const ::std::string& e,
               ::xml_schema::flags f)
{
  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::configuration (s, m, f));

  ::xsd::cxx::tree::error_handler< char > h;

  if (!::xsd::cxx::xml::dom::serialize (t, *d, e, h, f))
  {
    h.throw_if_failed< ::xsd::cxx::tree::serialization< char > > ();
  }
}

void
configuration (::xercesc::XMLFormatTarget& t,
               const ::config_t& s,
               ::xml_schema::error_handler& h,
               const ::xml_schema::namespace_infomap& m,
               const ::std::string& e,
               ::xml_schema::flags f)
{
  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::configuration (s, m, f));
  if (!::xsd::cxx::xml::dom::serialize (t, *d, e, h, f))
  {
    throw ::xsd::cxx::tree::serialization< char > ();
  }
}

void
configuration (::xercesc::XMLFormatTarget& t,
               const ::config_t& s,
               ::xercesc::DOMErrorHandler& h,
               const ::xml_schema::namespace_infomap& m,
               const ::std::string& e,
               ::xml_schema::flags f)
{
  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::configuration (s, m, f));
  if (!::xsd::cxx::xml::dom::serialize (t, *d, e, h, f))
  {
    throw ::xsd::cxx::tree::serialization< char > ();
  }
}

void
configuration (::xercesc::DOMDocument& d,
               const ::config_t& s,
               ::xml_schema::flags)
{
  ::xercesc::DOMElement& e (*d.getDocumentElement ());
  const ::xsd::cxx::xml::qualified_name< char > n (
    ::xsd::cxx::xml::dom::name< char > (e));

  if (n.name () == "configuration" &&
      n.namespace_ () == "")
  {
    e << s;
  }
  else
  {
    throw ::xsd::cxx::tree::unexpected_element < char > (
      n.name (),
      n.namespace_ (),
      "configuration",
      "");
  }
}

::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument >
configuration (const ::config_t& s,
               const ::xml_schema::namespace_infomap& m,
               ::xml_schema::flags f)
{
  ::xml_schema::dom::auto_ptr< ::xercesc::DOMDocument > d (
    ::xsd::cxx::xml::dom::serialize< char > (
      "configuration",
      "",
      m, f));

  ::configuration (*d, s, f);
  return d;
}

void
operator<< (::xercesc::DOMElement& e, const config_t& i)
{
  e << static_cast< const ::xml_schema::type& > (i);

  // username
  //
  {
    ::xercesc::DOMElement& s (
      ::xsd::cxx::xml::dom::create_element (
        "username",
        e));

    s << i.username ();
  }

  // server
  //
  {
    ::xercesc::DOMElement& s (
      ::xsd::cxx::xml::dom::create_element (
        "server",
        e));

    s << i.server ();
  }
}

#include <xsd/cxx/post.hxx>

// Begin epilogue.
//
//
// End epilogue.

