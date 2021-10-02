#
# test_codecmaps_cn.py
#   Codec mapping tests for PRC encodings
#

from test import multibytecodec_support
import unittest

class TestGB2312Map(multibytecodec_support.TestBase_Mapping,
                   unittest.TestCase):
    encoding = 'gb2312'
    mapfileurl = '/zip/.python/test/EUC-CN.TXT'

class TestGBKMap(multibytecodec_support.TestBase_Mapping,
                   unittest.TestCase):
    encoding = 'gbk'
    mapfileurl = '/zip/.python/test/CP936.TXT'

class TestGB18030Map(multibytecodec_support.TestBase_Mapping,
                     unittest.TestCase):
    encoding = 'gb18030'
    mapfileurl = '/zip/.python/test/gb-18030-2000.ucm'

if __name__ == "__main__":
    unittest.main()
