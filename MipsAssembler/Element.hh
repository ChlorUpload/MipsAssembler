#ifndef __H_ELEMENT__
#define __H_ELEMENT__

enum class ElementType
{
    TOKEN_CONST,
    TOKEN_REGISTER,
    TOKEN_LABEL,
    TOKEN_LABEL_ID,
    TOKEN_BRAKET_LEFT,
    TOKEN_BRAKET_RIGHT,
    TOKEN_COMMA,
    TOKEN_INST_ID,

    ADDRESS,
    INSTRUCTION,

    ELEMENT_NULL,
};

class Element
{
  protected:
    ElementType _type;

  public:
    virtual ElementType getType()
    {
        return _type;
    }
    Element() : _type { ElementType::ELEMENT_NULL } {}
    Element(ElementType type) : _type { type } {}
};

struct NullElement : public Element
{
    virtual ElementType getType()
    {
        return ElementType::ELEMENT_NULL;
    }
};

#endif