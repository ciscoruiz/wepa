#include <libxml/tree.h>
#include <libxml/xmlwriter.h>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Compiler.hpp>

using namespace coffee;

xml::Node::Node(const char* name) : Wrapper()
{
   setHandler(xmlNewNode(NULL, BAD_CAST name));
   setDeleter(xmlFreeNode);
   setNameExtractor(nameExtractor);
}

xml::Node::Node(_xmlNode* handler) : Wrapper(handler)
{
   setNameExtractor(nameExtractor);
}

// static
const char* xml::Node::nameExtractor(const Handler handler)
   noexcept
{
   return(const char*)(handler->name);
}

const std::string& xml::Node::getText() const
   throw(adt::RuntimeException)
{
   if(hasText() == false) {
      COFFEE_THROW_EXCEPTION("Node '" << getName() << "' does not have any TEXT item");
   }

   return m_text.getValue();
}

void xml::Node::addAttribute(std::shared_ptr<Attribute> attribute)
   throw(adt::RuntimeException)
{
   auto rr = m_attributes.insert(attribute);

   if(rr.second ==  false)
      COFFEE_THROW_EXCEPTION(attribute->getName() << " is already defined in node '" << getName() << "'");
}

std::shared_ptr<xml::Node> xml::Node::lookupChild(const char* name) const
   throw(adt::RuntimeException)
{
   std::shared_ptr<Node> result = searchChild(name);

   if(!result) {
      COFFEE_THROW_EXCEPTION("Child '" << name << "' is not registered in node '" << getName() << "'");
   }

   return result;
}

std::shared_ptr<xml::Node> xml::Node::searchChild(const char* name) const
   noexcept
{
   std::shared_ptr<Node> result;

   for(const_child_iterator ii = child_begin(), maxii = child_end(); ii != maxii; ++ ii) {
      const std::shared_ptr<Node>& child = get_child(ii);

      if(child->getName() == name) {
         result = child;
         break;
      }
   }

   return result;
}

std::shared_ptr<xml::Node> xml::Node::childAt(const size_t ii) const
   throw(adt::RuntimeException)
{
   if(ii >= children_size()) {
      COFFEE_THROW_EXCEPTION("There is not any child at position: " << ii);
   }

   return get_child(child_begin() + ii);
}

std::shared_ptr<xml::Node> xml::Node::lookupChild(const char* name)
   throw(adt::RuntimeException)
{
   std::shared_ptr<Node> result = searchChild(name);

   if(!result) {
      COFFEE_THROW_EXCEPTION("Child '" << name << "' is not registered in node '" << getName() << "'");
   }

   return result;
}

std::shared_ptr<xml::Node> xml::Node::searchChild(const char* name)
   noexcept
{
   std::shared_ptr<Node> result;

   for(child_iterator ii = child_begin(), maxii = child_end(); ii != maxii; ++ ii) {
      std::shared_ptr<Node>& child = get_child(ii);

      if(child->getName() == name) {
         result = child;
         break;
      }
   }

   return result;
}

std::shared_ptr<xml::Node> xml::Node::childAt(const size_t ii)
   throw(adt::RuntimeException)
{
   if(ii >= children_size()) {
      COFFEE_THROW_EXCEPTION("There is not any child at position: " << ii);
   }

   return get_child(child_begin() + ii);
}

std::shared_ptr<xml::Node> xml::Node::createChild(const char* name)
   throw(adt::RuntimeException)
{
   if(getHandler() == NULL)
      COFFEE_THROW_EXCEPTION("Can not create a child on an empty XML node");

   std::shared_ptr<xml::Node> result = std::make_shared<xml::Node>(xmlNewChild(getHandler(), NULL, BAD_CAST name, NULL));

   addChild(result);

   return result;
}

std::shared_ptr<xml::Attribute> xml::Node::createAttribute(const char* name, const char* value)
   throw(adt::RuntimeException)
{
   if(getHandler() == NULL)
      COFFEE_THROW_EXCEPTION("Can not create an attribute on an empty XML node");

   std::shared_ptr<xml::Attribute> result = std::make_shared<xml::Attribute>(xmlNewProp(getHandler(), BAD_CAST name, BAD_CAST value));

   addAttribute(result);

   return result;
}

void xml::Node::createText(const char* text)
   throw(adt::RuntimeException)
{
   if(getHandler() == NULL)
      COFFEE_THROW_EXCEPTION("Can not create an attribute on an empty XML node");

   Handler textNode = xmlNewText(BAD_CAST text);

   xmlAddChild(getHandler(), textNode);
   setText(text);
}

std::shared_ptr<xml::Attribute> xml::Node::lookupAttribute(const char* name) const
   throw(adt::RuntimeException)
{
   std::shared_ptr<Attribute> result = searchAttribute(name);

   if(!result) {
      COFFEE_THROW_EXCEPTION("Attribute '" << name << "' is not registered in node '" << getName() << "'");
   }

   return result;
}

std::shared_ptr<xml::Attribute> xml::Node::searchAttribute(const char* name) const
   noexcept
{
   std::shared_ptr<xml::Attribute> result;

   for(const_attribute_iterator ii = attribute_begin(), maxii = attribute_end(); ii != maxii; ++ ii) {
      const std::shared_ptr<Attribute>& attribute = get_attribute(ii);

      if(attribute->getName() == name) {
         result = attribute;
         break;
      }
   }

   return result;
}

void xml::Node::compile(xml::Compiler& compiler) const
   throw(adt::RuntimeException)
{
   int rc;

   xmlNsPtr nameSpace = getHandler()->ns;

   const unsigned char* name = BAD_CAST(getName().c_str());

   if(m_text.isNull() == true) {
      if(nameSpace == NULL)
         rc = xmlTextWriterStartElement(compiler, name);
      else
         rc = xmlTextWriterStartElementNS(compiler, BAD_CAST(nameSpace->prefix), name, BAD_CAST(nameSpace->href));

      if(rc < 0)
         COFFEE_THROW_EXCEPTION("Can not compile node " << getName());

      for(const_attribute_iterator ii = attribute_begin(), maxii = attribute_end(); ii != maxii; ++ ii) {
         const std::shared_ptr<Attribute>& attribute = get_attribute(ii);
         attribute->compile(compiler);
      }

      for(const_child_iterator ii = child_begin(), maxii = child_end(); ii != maxii; ++ ii) {
         const std::shared_ptr<Node>& child = get_child(ii);
         child->compile(compiler);
      }

      if(xmlTextWriterEndElement(compiler) < 0)
         COFFEE_THROW_EXCEPTION("Can not compile node " << getName());
   }
   else {
      const unsigned char* text = BAD_CAST(compiler.encode(m_text.getValue()));

      if(nameSpace == NULL)
         rc = xmlTextWriterWriteElement(compiler, name, text);
      else
         rc = xmlTextWriterWriteElementNS(compiler, BAD_CAST(nameSpace->prefix), name, BAD_CAST(nameSpace->href), text);

      if(rc < 0)
         COFFEE_THROW_EXCEPTION("Can not compile node " << getName());
   }
}

