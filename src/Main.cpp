
#include "Engine.h"
#include "GLSLGenerator.h"
#include "HLSLParser.h"
#include "HLSLTree.h"

/*
Exposes: parseHLSL
uses HLSLParser.onSuccess and HLSLParser.onError in JS scope
For more info on bindings, see
http://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html#interacting-with-code-call-function-pointers-from-c
*/

#include <emscripten/val.h>

#define str(str) std::string(str)

extern "C" {

    int parseHLSL(char* source, char* entryName, char* type)
    {
        using namespace M4;

        const char* fileName = "filename.hlsl";

        emscripten::val HLSLParserBridge = emscripten::val::global("HLSLParser");

        // emscripten::val console = emscripten::val::global("console");
        // console.call<void>("log", str("Source: "), str(source));
        // console.call<void>("log", str("Filename: "), str(fileName));
        // console.call<void>("log", str("type: "), str(type));
        // console.call<void>("log", str("Entry: "), str(entryName));

        // fragment shader by default
        GLSLGenerator::Target target = GLSLGenerator::Target_FragmentShader;
        if (String_Equal(type, "vs")) {
            target = GLSLGenerator::Target_VertexShader;
        }

        // Parse input file
        Allocator allocator;
        HLSLParser parser(&allocator, fileName, source, strlen(source));
        HLSLTree tree(&allocator);
        if (!parser.Parse(&tree))
        {
            // Log_Error("Parsing failed, aborting");
            HLSLParserBridge.call<void>("onError", str("parsing failed"));
            return 1;
        }

        // Generate output
        GLSLGenerator generator(&allocator);
        if (!generator.Generate(&tree, target, entryName)) {
            // Log_Error("Generation error.\n");
            HLSLParserBridge.call<void>("onError", str("code generation failed"));
            return EXIT_FAILURE;
        }

        const char *result = generator.GetResult();
        HLSLParserBridge.call<void>("onSuccess", str(result));

        // Log_Error(result);

        return 0;
    }
}
