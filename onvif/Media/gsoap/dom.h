/*

dom.h

gSOAP DOM interface. Use #import "dom.h" in gSOAP header files to add DOM-based
xsd__anyType and xsd__anyAttribute types. Automatic with wsdl2h option -d.

gSOAP XML Web services tools
Copyright (C) 2001-2008, Robert van Engelen, Genivia, Inc. All Rights Reserved.
This part of the software is released under one of the following licenses:
GPL, the gSOAP public license, or Genivia's license for commercial use.
--------------------------------------------------------------------------------
gSOAP public license.

The contents of this file are subject to the gSOAP Public License Version 1.3
(the "License"); you may not use this file except in compliance with the
License. You may obtain a copy of the License at
http://www.cs.fsu.edu/~engelen/soaplicense.html
Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Initial Developer of the Original Code is Robert A. van Engelen.
Copyright (C) 2000-2008 Robert A. van Engelen, Genivia inc. All Rights Reserved.
--------------------------------------------------------------------------------
GPL license.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA

Author contact information:
engelen@genivia.com / engelen@acm.org

This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia, Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

/**

@mainpage The gSOAP level-2 DOM parser

The gSOAP level-2 DOM parser features "smart" XML namespace handling and can be
used to mix gSOAP XML serializers with plain XML parsing. The DOM parser is
also an essential component of the wsse plugin to verify digital signatures.

The DOM parser is not a stand-alone application. The DOM parser is integrated
with the SOAP engine to populate a node set and to render a node set in XML.

Two files are needed to work with DOM node sets:

- dom.h to use a DOM node set in a gSOAP header file with service definitions.
- dom.cpp (or dom.c) to be linked with your application code.

@section dom_1 Declaring DOM Node Sets in a gSOAP Header File

To use the DOM node set with the data structures defined in a gSOAP header
file, import the dom.h file into your header file:

@code
    #import "dom.h"
@endcode

By importing dom.h two special data types @ref xsd__anyType and @ref
xsd__anyAttribute are available representing a hierarchical DOM node set of
elements and attributes, respectively. The DOM node element and attribute data
structures can be used within structs, classes, STL containers, and as
arguments of service operations. For example:

@code
    #import "dom.h"
    #import "wsu.h"
    class ns__myProduct
    { public:
	@char*               wsu__Id;
	@xsd__anyAttribute   atts;
	_wsu__Timestamp*     wsu__Timestamp;
        char*                name;
	int                  SKU;
	double               price;
	xsd__anyType*	     elts;
                             ns__myProduct();
                             ~ns__myProduct();
    };
@endcode

It is important to declare the @ref xsd__anyType at the end of the struct or
class, since the DOM parser consumes any XML element (the field name, 'elts' in
this case, is irrelavant).  Thus, the other fields must be defined first to
ensure they are populated first before the DOM node set is populated with any
non-previously matched XML element. Likewise, the @ref xsd__anyAttribute member
should be placed after the other attributes.

Note that we also imported wsu.h as an example to show how to add a wsu:Id
attribute to a struct or class if we want to digitally sign instances, and how
to add a standardized wsu:Timestamp element to record creation and expiration
times.

To compile, run soapcpp2 (with -Iimport) and compile your code by linking
dom.cpp (or dom.c for C). Note that the DOM data structures are declared in
stdsoap2.h, while the DOM operations are defined in dom.cpp (or dom.c for C).

Methods to populate and traverse DOM node sets will be explained later. First,
let's take a look at parsing and generating XML documents.

@section dom_2 Parsing and Generating XML

The following examples assume that the soapcpp2 compiler was used on a header
file (just the dom.h file will do) and the generated soapC.cpp or (soapC.c for
C) code was compiled and linked with dom.cpp (or dom.c for C), stdsoap2.cpp (or
stdsoap2.c) and the example application code. The generated namespace table
should also be used, since the namespace bindings are relevant for consuming
and producing XML for DOM node sets. Therefore, each of the example codes
in this documentation is assumed to start with the following two includes:

@code
    #include "soapH.h"  // generated by soapcpp2
    #include "ns.nsmap" // a namespace table with the XML namespace used
@endcode

The C++ std::iostream operators are overloaded to parse XML octet streams into
node sets and to emit XML from node sets:

@code
    soap_dom_element dom;
    dom.soap = soap_new1(SOAP_DOM_TREE | SOAP_C_UTFSTRING);
    cin >> dom; // parse XML
    if (dom.soap->error)
      ... // parse error
    cout << dom; // display XML
    if (dom.soap->error)
      ... // output error
    soap_destroy(dom.soap);
    soap_end(dom.soap);
    soap_done(dom.soap);
    free(dom.soap);
@endcode

In the example above we copied an XML document from stdin to stdout.

In C we use the DOM "serializers" to accomplish this as follows:

@code
    soap_dom_element dom;
    dom.soap = soap_new1(SOAP_DOM_TREE | SOAP_C_UTFSTRING);
    dom.soap->recvfd = stdin;
    if (soap_begin_recv(dom.soap)
     || NULL != soap_in_xsd__anyType(dom.soap, NULL, &dom, NULL)
     || soap_end_recv(dom.soap))
      ... // parse error
    dom.soap->sendfd = stdout;
    if (soap_begin_send(dom.soap))
      ... // output error
    dom.soap->ns = 2; // note: must use this to omit namespaces table dumping
    if (soap_out_xsd__anyType(dom.soap, NULL, 0, &dom, NULL)
     || soap_end_send(dom.soap))
      ... // output error
    soap_end(dom.soap);
    soap_done(dom.soap);
    free(dom.soap);
@endcode

The SOAP_DOM_NODE flag is used to instruct the parser to populate a DOM node
set with deserialized C and C++ data structures using the data type's
deserializers that were generated with soapcpp2 from a header file with the
data type declarations. Suppose for example that the following header file was
used (in fact, this declaration appears in wsu.h):

@code
    typedef struct _wsu__Timestamp
    {	@char*	wsu__Id;
    	char*	Created;
	char*	Expires;
    } _wsu__Timestamp;
@endcode

Note that the leading underscore of the type name indicates an XML element
definition (rather than a complexType definition), so the name of the data type
is relevant when comparing XML element tags to C/C++ data types by the
deserializers.

When an XML document is parsed with one or more <wsu:Timestamp> elements, the
DOM will be automatically populated with the _wsu__Timestamp objects. Suppose
the XML document root is a <wsu:Timestamp>, then the root node of the DOM is a
_wsu__Timestamp object:

@code
    soap_dom_element dom;
    dom.soap = soap_new1(SOAP_DOM_NODE);
    cin >> dom; // parse XML
    if (dom.soap->error)
      ... // parse error
    if (dom.type == SOAP_TYPE__wsu__Timestamp)
    { _wsu__Timestamp *t = (_wsu__Timestamp*)dom.node;
      cout << "Start " << (t->Created ? t->Created : "")
           << " till " << (t->Expires ? t->Expires : "") << endl;
    }
@endcode

Note that the soapcpp2 compiler generates a unique type identification constant
SOAP_TYPE_X for each data type X, which is used to determine the node's type in
the example above.

When objects occur deeper within the DOM node set then the DOM tree should be
traversed. This subject will be discussed next.

@section dom_3 Traversing a DOM Node Set

The DOM node set is traversed with a C++ iterator or with the C functions @ref
soap_dom_next_element and @ref soap_dom_next_attribute. For example, to walk
the node set visiting nodes in the same order as they appeared in the document,
use:

@code
    soap_dom_element dom;
    dom.soap = soap_new1(SOAP_DOM_TREE | SOAP_C_UTFSTRING);
    ...
    for (soap_dom_element::iterator iter = dom.begin(); iter != dom.end(); ++iter)
      for (soap_dom_attribute::iterator attr = (*iter).atts.begin(); attr != (*iter).atts.end(); ++attr)
        ...
@endcode

In C code, use:

@code
    soap_dom_element dom, *iter;
    soap_dom_attribute *attr;
    dom.soap = soap_new1(SOAP_DOM_TREE | SOAP_C_UTFSTRING);
    ...
    for (iter = &dom; iter; iter = soap_dom_next_element(iter))
      for (attr = iter->atts; attr; attr = soap_dom_next_attribute(attr))
         ...
@endcode

The @ref soap_dom_element and @ref soap_dom_attribute structs form essentially
linked lists, so it would not be too difficult to write your own tree walkers:

- @ref soap_dom_element::prnt points to the parent soap_dom_element node.
- @ref soap_dom_element::elts points to the linked list of child element nodes.
- @ref soap_dom_element::atts points to the linked list of attribute nodes.

The linked lists of sibling elements nodes and attribute nodes are respectively:

- @ref soap_dom_element::next points to the next sibling element node.
- @ref soap_dom_attribute::next points to the next attribute in the attribute
  list of an element node.

Note that for a root node, the @ref soap_dom_element::prnt and @ref
soap_dom_element::next are both NULL.

Tag names of elements and attributes are stored in @ref soap_dom_element::name
and @ref soap_dom_attribute::name strings, respectively. The names are UTF-8
encoded.

XML namespace bindings are explicitly propagated throughout the DOM node set
for those elements and attributes that are namespace qualified (either with a
namespace prefix or when they occur in a xmlns default namespace scope). The
namespaces are stored in the @ref soap_dom_element::nstr and @ref
soap_dom_attribute::nstr strings. The following example shows how to traverse a
DOM node set and print the elements with their namespace URIs when present:

@code
    soap_dom_element dom;
    dom.soap = soap_new1(SOAP_DOM_TREE | SOAP_C_UTFSTRING);
    cin >> dom;
    for (soap_dom_element::iterator iter = dom.begin(); iter != dom.end(); ++iter)
    { cout << "Element " << (*iter).name;
      if ((*iter).nstr)
        cout << " has namespace " << (*iter).nstr;
      cout << endl;
    }
    soap_destroy(dom.soap);
    soap_end(dom.soap);
    soap_done(dom.soap);
    free(dom.soap);
@endcode

Text content of a node is stored in the @ref soap_dom_element::data string in
UTF-8 format. This string is populated if the SOAP_C_UTFSTRING flag was set.
Otherwise the data content will be stored in the @ref soap_dom_element::wide
wide-character string.

The following example prints those element nodes that have text content
(in UTF-8 format):

@code
    soap_dom_element dom;
    ...
    for (soap_dom_element::iterator iter = dom.begin(); iter != dom.end(); ++iter)
    { cout << "Element " << (*iter).name;
      if ((*iter).data)
        cout << " = " << (*iter).data;
      cout << endl;
    }
    ...
@endcode

When a DOM node set contains deserialized objects (enabled with the
SOAP_DOM_NODE flag), the @ref soap_dom_element::type and @ref
soap_dom_element::node values are set:

@code
    soap_dom_element dom;
    ...
    for (soap_dom_element::iterator iter = dom.begin(); iter != dom.end(); ++iter)
    { cout << "Element " << (*iter).name;
      if ((*iter).type)
        cout << "Element " << (*iter).name << " contains a deserialized object" << endl;
      cout << endl;
    }
    ...
@endcode

The @ref soap_dom_element::type is 0 or a SOAP_TYPE_X constant, where X is the
name of the deserialized type. The @ref soap_dom_element::node points to the
deserialized object. If this is a char* string, it points directly to the
character sequence.

Note: the SOAP_DOM_TREE flag restricts the parser to DOM content only, so
deserializers is not used. When the SOAP_DOM_TREE flag is not used, an
appropriate deserializer MAY be used by gSOAP when an element contains an id
attribute and gSOAP can determine the type from the id attribute reference
and/or the xsi:type attribute of an element. 

@section dom_4 Searching

Common operations on DOM node sets in level-2 DOM parsers are searching and
filtering.

For C++ code, the built-in @ref soap_dom_element::iterator can be used to
search for matching element nodes. C programmers are out of luck as they should
write looping code to search for nodes explicitly.

The @ref soap_dom_element::find method returns a search iterator. The method
takes an optional namespace URI and element name to match elements in the DOM
node set.  For example, to iterate over all "product" elements:

@code
    soap_dom_element dom;
    ...
    for (soap_dom_element::iterator iter = dom.find(NULL, "product"); iter != dom.end(); ++iter)
      cout << "Element " << (*iter).name << endl;
    ...
@endcode

To iterate over all elements in a particular namespace:

@code
    soap_dom_element dom;
    ...
    for (soap_dom_element::iterator iter = dom.find("http://www.w3.org/2001/XMLSchema", NULL); iter != dom.end(); ++iter)
      cout << "Element " << (*iter).name << endl;
    ...
@endcode

Since namespaces may have different version, a '*' wildcard can be used with
the namespace string. Likewise, tag names may be namespace qualified with
prefixes that are not relevant to the search:

@code
    soap_dom_element dom;
    ...
    for (soap_dom_element::iterator iter = dom.find("http://www.w3.org/*XMLSchema", "*:schema"); iter != dom.end(); ++iter)
      cout << "Element " << (*iter).name << endl;
    ...
@endcode

This searches for qualified elements in one of the XSD namespaces.

@section dom_5 Constructing DOM Node Sets

The @ref soap_dom_element::set and @ref soap_dom_element::add methods are used
to decorate a DOM node set with child element nodes and attribute nodes.
Application data with serializers can be incorporated in the node set as well.

The following examples are shown in C++.  C programmers can use the @ref
soap_dom_element:elts list and @ref soap_dom_elements::atts list to add child
nodes and attribute nodes, respectively.

@code
    soap_dom_element dom;
    dom.soap = soap_new1(SOAP_C_UTFSTRING | SOAP_XML_INDENT);
    const char *myURI = "http://www.mydomain.com/myproducts";
    ns__myProduct product();
    product.soap_default(dom.soap); // method generated by soapcpp2
    product.name = "Ernie";
    product.SKU = 123;
    product.price = 9.95;
    dom.set(myURI, "list");
    dom.add(soap_dom_attribute(dom.soap, myURI, "version", "0.9"));
    dom.add(soap_dom_element(dom.soap, myURI, "documentation", "List of products"));
    dom.add(soap_dom_element(dom.soap, myURI, "product", &product, SOAP_TYPE_ns__myProduct);
    cout << dom;
    ...
@endcode

Assuming that myURI is associated with namespace prefix "ns" in the namespace
table, the rendition is

@code
<?xml version="1.0" encoding="UTF-8"?>
<ns:list
  xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
  xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:ns="http://domain/schemas/product.xsd"
  version="0.9" >
	<ns:documentation>List of products</ns:documentation>
	<ns:product>
		<name>Ernie</name>
		<SKU>123</SKU>
		<price>9.95</price>
	</ns:product>
</ns:list>
@endcode

Note that the namespace table content is "dumped" into the XML rendition.

The global namespace mapping table "namespaces[]" contains the namespace
bindings that should be meaningful to the application. The soap context can be
set to a new table as follows:

@code
    Namespace myNamespaces[] = { { "ns", "..." }, ... , { NULL } };
    soap_dom_element dom;
    dom.soap = soap_new1(SOAP_C_UTFSTRING | SOAP_XML_INDENT);
    dom.soap->namespaces = myNamespaces;
@endcode

To produce cleaner XML, use the SOAP_XML_CANONICAL flag to initiate the soap
context:

@code
<ns:list xmlns:ns="http://domain/schemas/product.xsd" version="0.9" >
	<ns:documentation>List of products</ns:documentation>
	<ns:product>
		<name>Ernie</name>
		<SKU>123</SKU>
		<price>9.95</price>
	</ns:product>
</ns:list>
@endcode

Note that the xmlns bindings are rendered automatically. When parsing an XML
document, xmlns bindings are not added to the attribute node set. The @ref
soap_dom_element::nstr and @ref soap_dom_attribute::nstr namespace strings are
set to retain namespace URIs. The XML rendering algorithm uses the namespace
strings to add xmlns bindings that are not already in the namespace table.

When it is desirable to render XML exactly as represented in the DOM node set,
e.g. when xmlns bindings are explicitly included in the attribute node set, use
the SOAP_DOM_ASIS flag:

@code
    soap_dom_element dom;
    dom.soap = soap_new1(SOAP_C_UTFSTRING | SOAP_DOM_ASIS);
@endcode

@section dom_6 Example

The gSOAP header file below imports DOM and declares xsd:float to enable
serializing floats embedded within DOM node sets and deserializing floats to
populate DOM node sets:

@code
    #import "dom.h"
    typedef float xsd__float;
@endcode

Consider invoking the XMethods delayed stock quote service to obtain a stock
quote. The float deserializer is used to store the floating-point value of a
stock given that the <Result> element has an xsi:type="xsd:float" attribute.

@code
    struct soap *soap = soap_new1(SOAP_C_UTFSTRING | SOAP_DOM_NODE);
    soap_dom_element envelope(soap, "http://schemas.xmlsoap.org/soap/envelope/", "Envelope");
    soap_dom_element body(soap, "http://schemas.xmlsoap.org/soap/envelope/", "Body");
    soap_dom_attribute encodingStyle(soap, "http://schemas.xmlsoap.org/soap/envelope/", "encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/");
    soap_dom_element request(soap, "urn:xmethods-delayed-quotes", "getQuote");
    soap_dom_element symbol(soap, NULL, "symbol", "IBM");
    soap_dom_element response(soap);
    envelope.add(body);
    body.add(encodingStyle);
    body.add(request);
    request.add(symbol);
    cout << "Request message:" << endl << envelope << endl;
    if (soap_connect(soap, "http://services.xmethods.net/soap", "")
     || soap_out_xsd__anyType(soap, NULL, 0, &envelope, NULL)
     || soap_end_send(soap)
     || soap_begin_recv(soap)
     || NULL != soap_in_xsd__anyType(soap, NULL, &response, NULL)
     || soap_end_recv(soap)
     || soap_closesock(soap))
    { soap_print_fault(soap, stderr);
      soap_print_fault_location(soap, stderr);
    }
    else
    { cout << "Response message:" << endl << response << endl;
      for (soap_dom_element::iterator walker = response.find(SOAP_TYPE_xsd__float); walker != response.end(); ++walker)
        cout << "Quote = " << *(xsd__float*)(*walker).node << endl;
    }
    soap_destroy(soap);
    soap_end(soap);
    soap_done(soap);
    free(soap);
@endcode

@section dom_7 Summary

The DOM parser needs a soap context to allocate nodes:

@code
    soap_dom_element dom;
    dom.soap = soap_new1(... flags ...);
    ...
    soap_destroy(dom.soap);
    soap_end(dom.soap);
    soap_done(dom.soap);
    soap_free(dom.soap);
@endcode

The nodes are removed with soap_destroy (for C++) and soap_end. The soap_done
function should only be used before the soap context is deallocated.

The soap context flags that control the parsing and rendition of XML are:

- (no flag): only elements with an id attribute are deserialized as C/C++ data
  types (when a deserializer is available). XML elements with character data
  are deserialized into the @ref soap_dom_element::wide field.
- SOAP_C_UTFSTRING: store character data in UTF-8 format in @ref
  soap_dom_element::data.
- SOAP_C_MBSTRING: store character data in multi-byte format in @ref
  soap_dom_element::data, where the decoding depends on the current
  localication. The platform must support MB strings (HAVE_MBTOWC).
- SOAP_DOM_TREE: prevents deserialization of C/C++ data structures into the
  DOM.
- SOAP_DOM_NODE: attempt to deserialize C/C++ data structures when a
  deserializer is available. A deserializer is selected based on the element
  name or the xsi:type attribute.
- SOAP_DOM_ASIS: render XML "as is", i.e. do not insert xmlns bindings for URIs
  stored in nstr. Assumes the DOM is self-contained.
- SOAP_XML_INDENT: render XML with indent.
- SOAP_XML_CANONICAL: render XML in exc-c14n form.

The DOM traversal operations:

- @ref soap_dom_next_element returns the next element in an in-order traversal.
- @ref soap_dom_next_attribute returns the next attribute of a node.

The @ref soap_dom_element fields:

- @ref soap_dom_element::next pointer to next sibling in list.
- @ref soap_dom_element::prnt pointer to parent node.
- @ref soap_dom_element::elts pointer to list of child element nodes.
- @ref soap_dom_element::atts pointer to list of attribute nodes.
- @ref soap_dom_element::nstr optional namespace string of this node.
- @ref soap_dom_element::name the name of the element node (with optional
  prefix).
- @ref soap_dom_element::data optional character data in UTF-8 format.
- @ref soap_dom_element::wide optional character data in wide string format.
- @ref soap_dom_element::type optional SOAP_TYPE_X type of a C/C++ data
  structure stored with this node.
- @ref soap_dom_element::node optional pointer to the C/C++ data structure
  stored with this node.
- @ref soap_dom_element::head optional leading whitespace to the start tag.
- @ref soap_dom_element::tail optional leading whitespace to the end tag.
- @ref soap_dom_element::soap the soap context that manages this node.

The @ref soap_dom_element types:

- @ref soap_dom_element::iterator

The @ref soap_dom_element methods:

- @ref soap_dom_element::set(nstr, name);
- @ref soap_dom_element::set(data);
- @ref soap_dom_element::set(node, type);
- @ref soap_dom_element::add(soap_dom_element);
- @ref soap_dom_element::add(soap_dom_attribute);
- @ref soap_dom_element::begin();
- @ref soap_dom_element::end();
- @ref soap_dom_element::find(nstr, name);
- @ref soap_dom_element::find(type);
- @ref soap_dom_element::unlink();

The @ref soap_dom_element constructors:

- @ref soap_dom_element();
- @ref soap_dom_element(soap);
- @ref soap_dom_element(soap, nstr, name);
- @ref soap_dom_element(soap, nstr, name, data);
- @ref soap_dom_element(soap, nstr, name, node, type);

The @ref soap_dom_attribute fields:

- @ref soap_dom_attribute::next pointer to next attribute node in list.
- @ref soap_dom_attribute::nstr optional namespace string of this node.
- @ref soap_dom_attribute::name the name of the attribute (with optional
  prefix).
- @ref soap_dom_attribute::data optional character data in UTF-8 format.
- @ref soap_dom_attribute::soap the soap context that manages this node.

The @ref soap_dom_attribute types:

- @ref soap_dom_attribute::iterator

The @ref soap_dom_attribute methods:

- @ref soap_dom_attribute::set(nstr, name);
- @ref soap_dom_attribute::set(data);
- @ref soap_dom_attribute::begin();
- @ref soap_dom_attribute::end();
- @ref soap_dom_attribute::find(nstr, name);
- @ref soap_dom_attribute::unlink();

The @ref soap_dom_attribute constructors:

- @ref soap_dom_attribute();
- @ref soap_dom_attribute(soap);
- @ref soap_dom_attribute(soap, nstr, name, data);

*/

/// @brief The custom serializer for DOM element nodes is represented by xsd__anyType.
#ifndef LINUX_NAS
extern typedef struct soap_dom_element xsd__anyType;

/// @brief The custom serializer for DOM attribute nodes is represented by xsd__anyAttribute.
extern typedef struct soap_dom_attribute xsd__anyAttribute;
#endif
