#ifndef __H_SEMANTIC_ANALYZER__
#define __H_SEMANTIC_ANALYZER__
#include "Tokenizer.hh"
#include <Vector>
#include <string>

class SemanticAnalyzer
{
  private:
    std::vector<std::string> _labels;

  public:
    SemanticAnalyzer(Tokenizer& tokenizer) {
        tokenizer.begin();
    }
};

#endif