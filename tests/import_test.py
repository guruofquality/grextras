# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest

class ImportTest(unittest.TestCase):

    def test_import(self):
        import gras
        import grextras
        #print dir(grextras)
        #print dir(grextras.Delay)
        x = grextras.Delay(1)
        print dir(x)
        print x.to_string()
        print x

        tb = gras.TopBlock('fuck')
        tb.connect(x, x)


if __name__ == '__main__':
    unittest.main()
