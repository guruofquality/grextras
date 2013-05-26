# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy

SOURCE = """
#include <stdio.h>
#include <iostream>
int main(void)
{
    std::cout << "hey you guysers\\n";
    printf("hello\\n");
    return 0;
}
"""

class test_clang_block(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_foo(self):
        params = grextras.ClangBlockParams()
        #params.flags.append('-stdlib=libstdc++')
        params.code = SOURCE
        cb = grextras.ClangBlock(params)

if __name__ == '__main__':
    unittest.main()

