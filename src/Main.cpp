
#include "Engine.h"
#include "GLSLGenerator.h"
#include "HLSLParser.h"
#include "HLSLTree.h"

/*
TODO
- type
- filename
- file contents
- entry name

parseHLSL2GLSL
*/

extern "C" {

    int parse(char* source, char* fileName, char* entryName)
    {
        using namespace M4;

        Log_Error(source);
        Log_Error("..fileName...\n");

        GLSLGenerator::Target target = GLSLGenerator::Target_FragmentShader;

        // Parse input file
        Allocator allocator;
        HLSLParser parser(&allocator, fileName, source, strlen(source));
        HLSLTree tree(&allocator);
        if (!parser.Parse(&tree))
        {
            Log_Error("Parsing failed, aborting");
            return 1;
        }

        // Generate output
        GLSLGenerator generator(&allocator);
        if (!generator.Generate(&tree, target, entryName)) {
            Log_Error("Generation error.\n");
            return EXIT_FAILURE;
        }
        Log_Error(generator.GetResult());

        return 0;
    }

}

