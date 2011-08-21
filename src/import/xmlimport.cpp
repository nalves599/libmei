//
//  xmlimport.cpp
//  libmei
//
//  Created by Andrew Hankinson on 11-08-18.
//  Copyright 2011 McGill University. All rights reserved.
//

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <libxml/xmlreader.h>

#include "xmlimport.h"
#include "meidocument.h"

using std::string;
using std::cout;
using std::endl;

using mei::MeiDocument;
using mei::MeiElement;
using mei::MeiFactory;
using mei::XmlImport;

MeiDocument* mei::XmlImport::documentFromFile(const char* filename) {
    xmlDoc *doc = NULL;
    doc = xmlReadFile(filename, NULL, 0);
    XmlImport *mfile = new XmlImport(doc);
    return mfile->getMeiDocument();
}

//MeiDocument* XmlImport::documentFromStream(string xmlstream) {
//  MeiDocument* mdoc;
//
//  return mdoc;
//}
//
//MeiDocument* XmlImport::documentFromText(string text) {
//  MeiDocument* mdoc;
//
//  return mdoc;
//}

XmlImport::XmlImport(xmlDoc *d) {
    // at construction time parse the xmlNode out into the MeiDocument.
    this->xmlMeiDocument = d;
    this->init();
}

void XmlImport::init() {
    this->rootXmlNode = xmlDocGetRootElement(this->xmlMeiDocument);
    this->rootMeiElement = this->xmlNodeToMeiElement(this->rootXmlNode);
    MeiDocument *doc = new MeiDocument("test", "UTF-8");
    doc->setRootElement(this->rootMeiElement);    
    this->meiDocument = doc; 
}

mei::XmlImport::~XmlImport() {
    xmlCleanupParser();
}

MeiDocument* XmlImport::getMeiDocument() {
    return this->meiDocument;
}

MeiElement* XmlImport::xmlNodeToMeiElement(xmlNode *el) {
    
    if (!MeiFactory::inMap(string((const char*)el->name))) {
        return NULL;
    }
    
    MeiElement *obj;
    
    if (el->type == XML_ELEMENT_NODE) {
        obj = MeiFactory::createInstance((string((const char*)el->name)));   
    } else if (el->type == XML_TEXT_NODE) {
        obj = new MeiTextNode();
        obj->setValue(string((const char*)el->content));
    } else {
        return NULL;
    }
    
    // XML attributes and children. Text nodes will never have these.
    if (el->properties) {
        xmlAttr *curattr = NULL;
        for (curattr = el->properties; curattr; curattr = curattr->next) {
            string attrname = (const char*)curattr->name;
            // values are rendered as children of the attribute *facepalm*
            string attrvalue = (const char*)curattr->children->content;
            MeiAttribute *a = new MeiAttribute(attrname, attrvalue);
            
            if (curattr->ns) {
                if (attrname.compare("id") && string((const char*)curattr->ns->prefix).compare("xml")) {
                    obj->setId(attrvalue);
                }
                a->setPrefix(string((const char*)curattr->ns->prefix));
            }
            
            obj->addAttribute(a);
        }
    }
    xmlNodePtr child = el->children;
    while (child != NULL) {
        MeiElement* ch = xmlNodeToMeiElement(child);
        obj->addChild(ch);
        child = child->next;
    }
    return obj;
}

