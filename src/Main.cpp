
#include "Engine.h"
#include "GLSLGenerator.h"
#include "HLSLParser.h"
#include "HLSLTree.h"

#include <stdio.h>


M4::Allocator allocator;

static const char * readFile(const char * filename, size_t * lengthptr) {
    FILE * fp = fopen(filename, "rb");
    
    fseek(fp, 0, SEEK_END);
    size_t length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char * buffer = allocator.New<char>(length);

    fread(buffer, 1, length, fp);
    fclose(fp);

    if (lengthptr != NULL) {
        *lengthptr = length;
    }

    return buffer;
}


int main(int argc, char *argv[])
{
    const char * filename = "test.fx";

    // @@ Add input options.
    if (argc > 1) {
        filename = argv[1];
    }

    // Read input file.
    size_t length;
    const char * buffer = readFile(filename, &length);


    M4::HLSLTree tree(&allocator);
    M4::HLSLParser parser(&allocator, filename, buffer, length);

    if (!parser.Parse(&tree)) {
        M4::Log_Error("Parse error.\n");
        return EXIT_FAILURE;
    }

    M4::GLSLGenerator generator(&allocator);

    if (!generator.Generate(&tree, M4::GLSLGenerator::Target_VertexShader, "vertex_shader")) {
        M4::Log_Error("Generation error.\n");
        return EXIT_FAILURE;
    }

    puts(generator.GetResult());

    return EXIT_SUCCESS;
}


