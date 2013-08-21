/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* Copyright (C) 2001 by First Peer, Inc. All rights reserved.
** Copyright (C) 2002 Ximian, Inc.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission. 
**  
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
** OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE. */

#include "xmlrpc_config.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <xmlparser.h>
#else
#include <libxml/parser.h>
#endif

#include "xmlrpc-c/base.h"
#include "xmlrpc-c/base_int.h"
#include "xmlrpc-c/xmlparser.h"

/* Define the contents of our internal structure. */
struct _xml_element {
    struct _xml_element *_parent;
    char *_name;
    xmlrpc_mem_block _cdata;    /* char */
    xmlrpc_mem_block _children; /* xml_element* */
};

#define XMLRPC_ASSERT_ELEM_OK(elem) \
    XMLRPC_ASSERT((elem) != NULL && (elem)->_name != XMLRPC_BAD_POINTER)


/*=========================================================================
**  xml_element_new
**=========================================================================
**  Create a new xml_element. This routine isn't exported, because the
**  arguments are implementation-dependent.
*/

static xml_element *xml_element_new (xmlrpc_env *env, char *name)
{
    xml_element *retval;
    int name_valid, cdata_valid, children_valid;

    XMLRPC_ASSERT_ENV_OK(env);
    XMLRPC_ASSERT(name != NULL);

    /* Set up our error-handling preconditions. */
    retval = NULL;
    name_valid = cdata_valid = children_valid = 0;

    /* Allocate our xml_element structure. */
    retval = (xml_element*) malloc(sizeof(xml_element));
    XMLRPC_FAIL_IF_NULL(retval, env, XMLRPC_INTERNAL_ERROR,
			"Couldn't allocate memory for XML element");

    /* Set our parent field to NULL. */
    retval->_parent = NULL;
    
    /* Copy over the element name. */
    retval->_name = (char*) malloc(strlen(name) + 1);
    XMLRPC_FAIL_IF_NULL(retval->_name, env, XMLRPC_INTERNAL_ERROR,
			"Couldn't allocate memory for XML element");
    name_valid = 1;
    strcpy(retval->_name, name);

    /* Initialize a block to hold our CDATA. */
    XMLRPC_TYPED_MEM_BLOCK_INIT(char, env, &retval->_cdata, 0);
    XMLRPC_FAIL_IF_FAULT(env);
    cdata_valid = 1;

    /* Initialize a block to hold our child elements. */
    XMLRPC_TYPED_MEM_BLOCK_INIT(xml_element*, env, &retval->_children, 0);
    XMLRPC_FAIL_IF_FAULT(env);
    children_valid = 1;

 cleanup:
    if (env->fault_occurred) {
	if (retval) {
	    if (name_valid)
		free(retval->_name);
	    if (cdata_valid)
		xmlrpc_mem_block_clean(&retval->_cdata);
	    if (children_valid)
		xmlrpc_mem_block_clean(&retval->_children);
	    free(retval);
	}
	return NULL;
    } else {
	return retval;
    }
}


/*=========================================================================
**  xml_element_free
**=========================================================================
**  Blow away an existing element & all of its child elements.
*/

void xml_element_free (xml_element *elem)
{
    xmlrpc_mem_block *children;
    int size, i;
    xml_element **contents;

    XMLRPC_ASSERT_ELEM_OK(elem);

    free(elem->_name);
    elem->_name = XMLRPC_BAD_POINTER;
    xmlrpc_mem_block_clean(&elem->_cdata);

    /* Deallocate all of our children recursively. */
    children = &elem->_children;
    contents = XMLRPC_TYPED_MEM_BLOCK_CONTENTS(xml_element*, children);
    size = XMLRPC_TYPED_MEM_BLOCK_SIZE(xml_element*, children);
    for (i = 0; i < size; i++)
	xml_element_free(contents[i]);

    xmlrpc_mem_block_clean(&elem->_children);
    free(elem);
}


/*=========================================================================
**  Miscellaneous Accessors
**=========================================================================
**  Return the fields of the xml_element. See the header for more
**  documentation on each function works.
*/

const char *xml_element_name (const xml_element * const elem)
{
    XMLRPC_ASSERT_ELEM_OK(elem);
    return elem->_name;
}

/* The result of this function is NOT VALID until the end_element handler
** has been called! */
size_t xml_element_cdata_size (xml_element *elem)
{
    XMLRPC_ASSERT_ELEM_OK(elem);
    return XMLRPC_TYPED_MEM_BLOCK_SIZE(char, &elem->_cdata) - 1;
}

char *xml_element_cdata (xml_element *elem)
{
    XMLRPC_ASSERT_ELEM_OK(elem);
    return XMLRPC_TYPED_MEM_BLOCK_CONTENTS(char, &elem->_cdata);
}

size_t xml_element_children_size (const xml_element *const elem)
{
    XMLRPC_ASSERT_ELEM_OK(elem);
    return XMLRPC_TYPED_MEM_BLOCK_SIZE(xml_element*, &elem->_children);
}

xml_element **xml_element_children (const xml_element *const elem)
{
    XMLRPC_ASSERT_ELEM_OK(elem);
    return XMLRPC_TYPED_MEM_BLOCK_CONTENTS(xml_element*, &elem->_children);
}


/*=========================================================================
**  Internal xml_element Utility Functions
**=========================================================================
*/

static void xml_element_append_cdata (xmlrpc_env *env,
				      xml_element *elem,
				      char *cdata,
				      size_t size)
{
    XMLRPC_ASSERT_ENV_OK(env);
    XMLRPC_ASSERT_ELEM_OK(elem);    

    XMLRPC_TYPED_MEM_BLOCK_APPEND(char, env, &elem->_cdata, cdata, size);
}

/* Whether or not this function succeeds, it takes ownership of the 'child'
** argument.
** WARNING - This is the exact opposite of the usual memory ownership
** rules for xmlrpc_value! So please pay attention. */
static void xml_element_append_child (xmlrpc_env *env,
				      xml_element *elem,
				      xml_element *child)
{
    XMLRPC_ASSERT_ENV_OK(env);
    XMLRPC_ASSERT_ELEM_OK(elem);
    XMLRPC_ASSERT_ELEM_OK(child);
    XMLRPC_ASSERT(child->_parent == NULL);

    XMLRPC_TYPED_MEM_BLOCK_APPEND(xml_element*, env, &elem->_children,
                                  &child, 1);
    if (!env->fault_occurred)
	child->_parent = elem;
    else
        xml_element_free(child);
}


/*=========================================================================
**  Our parse context. We pass this around as libxml user data.
**=========================================================================
*/

typedef struct {
    xmlrpc_env *env;
    xml_element *root;
    xml_element *current;
} parse_context;


/*=========================================================================
**  LibXML Event Handler Functions
**=========================================================================
*/

static void
start_element(void *           const user_data,
              const xmlChar *  const name,
              const xmlChar ** const attrs ATTR_UNUSED) {

    parse_context *context;
    xml_element *elem, *new_current;

    XMLRPC_ASSERT(user_data != NULL && name != NULL);

    /* Get our context and see if an error has already occured. */
    context = (parse_context*) user_data;
    if (!context->env->fault_occurred) {

	/* Set up our error-handling preconditions. */
	elem = NULL;

	/* Build a new element. */
	elem = xml_element_new(context->env, (char *) name);
	XMLRPC_FAIL_IF_FAULT(context->env);

	/* Insert it in the appropriate place. */
	if (!context->root) {
	    context->root = elem;
	    context->current = elem;
	    elem = NULL;
	} else {
	    XMLRPC_ASSERT(context->current != NULL);

	    /* (We need to watch our error handling invariants very carefully
	    ** here. Read the docs for xml_element_append_child. */
	    new_current = elem;
	    xml_element_append_child(context->env, context->current, elem);
	    elem = NULL;
	    XMLRPC_FAIL_IF_FAULT(context->env);
	    context->current = new_current;
	}

 cleanup:
	if (elem)
	    xml_element_free(elem);
    }
}



static void
end_element(void *          const user_data,
            const xmlChar * const name ATTR_UNUSED) {

    parse_context *context;

    XMLRPC_ASSERT(user_data != NULL && name != NULL);

    /* Get our context and see if an error has already occured. */
    context = (parse_context*) user_data;
    if (!context->env->fault_occurred) {

	/* XXX - I think expat enforces these facts, but I want to be sure.
	** If one of these assertion ever fails, it should be replaced by a
	** non-assertion runtime error check. */
	XMLRPC_ASSERT(xmlrpc_streq(name, context->current->_name));
	XMLRPC_ASSERT(context->current->_parent != NULL ||
		      context->current == context->root);

	/* Add a trailing '\0' to our cdata. */
	xml_element_append_cdata(context->env, context->current, "\0", 1);
	XMLRPC_FAIL_IF_FAULT(context->env);	

	/* Pop our "stack" of elements. */
	context->current = context->current->_parent;

 cleanup:
	return;
    }
}

static void character_data (void *user_data, const xmlChar *s, int len)
{
    parse_context *context;

    XMLRPC_ASSERT(user_data != NULL && s != NULL && len >= 0);

    /* Get our context and see if an error has already occured. */
    context = (parse_context*) user_data;
    if (!context->env->fault_occurred) {

	XMLRPC_ASSERT(context->current != NULL);
	
	xml_element_append_cdata(context->env, context->current, (char *) s, len);
	XMLRPC_FAIL_IF_FAULT(context->env);

 cleanup:
	return;
    }
}


/*=========================================================================
**  LibXML Driver
**=========================================================================
**  XXX - We should allow the user to specify the encoding of our xml_data.
*/

static xmlSAXHandler sax_handler = {
    NULL,      /* internalSubset */
    NULL,      /* isStandalone */
    NULL,      /* hasInternalSubset */
    NULL,      /* hasExternalSubset */
    NULL,      /* resolveEntity */
    NULL,      /* getEntity */
    NULL,      /* entityDecl */
    NULL,      /* notationDecl */
    NULL,      /* attributeDecl */
    NULL,      /* elementDecl */
    NULL,      /* unparsedEntityDecl */
    NULL,      /* setDocumentLocator */
    NULL,      /* startDocument */
    NULL,      /* endDocument */
    start_element,       /* startElement */
    end_element,         /* endElement */
    NULL,      /* reference */
    character_data,      /* characters */
    NULL,      /* ignorableWhitespace */
    NULL,      /* processingInstruction */
    NULL,      /* comment */
    NULL,      /* warning */
    NULL,      /* error */
    NULL,      /* fatalError */
    NULL,      /* getParameterEntity */
    NULL,      /* cdataBlock */
    NULL,      /* externalSubset */
    1          /* initialized */
    
    /* Following are SAX2 fields. Any ifdef here? */ 

    ,NULL,     /* _private */
    NULL,      /* startElementNs */
    NULL,      /* endElementNs */
    NULL       /* serror */
};



void
xml_parse(xmlrpc_env *   const envP,
          const char *   const xmlData,
          size_t         const xmlDataLen,
          xml_element ** const resultPP) {

    parse_context context;
    xmlParserCtxt *parser;
    int err;

    XMLRPC_ASSERT_ENV_OK(envP);
    XMLRPC_ASSERT(xmlData != NULL && xmlDataLen >= 0);

    /* Set up our error-handling preconditions. */
    parser = NULL;
    context.root = NULL;
    
    /* Set up the rest of our parse context. */
    context.env     = envP;
    context.current = NULL;

    /* Set up our XML parser. */
    parser = xmlCreatePushParserCtxt(&sax_handler, &context, NULL, 0, NULL);
    XMLRPC_FAIL_IF_NULL(parser, envP, XMLRPC_INTERNAL_ERROR,
                        "Could not create expat parser");

    /* Parse our data. */
    err = xmlParseChunk(parser, xmlData, xmlDataLen, 1);
    if (err)
        XMLRPC_FAIL(envP, XMLRPC_PARSE_ERROR, "XML parsing failed");
    XMLRPC_FAIL_IF_FAULT(envP);

    /* Perform some sanity checks. */
    XMLRPC_ASSERT(context.root != NULL);
    XMLRPC_ASSERT(context.current == NULL);

    *resultPP = context.root;

 cleanup:
    if (parser) {
        if (parser->myDoc)
            xmlFreeDoc(parser->myDoc);
        xmlFreeParserCtxt(parser);
    }
    if (envP->fault_occurred) {
        if (context.root)
            xml_element_free(context.root);
    }
}
