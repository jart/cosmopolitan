import os
import shutil
import tempfile
import unittest
import subprocess


class SubprocessTest(unittest.TestCase):
    def test_execve(self):
        tmp_dir = tempfile.mkdtemp()
        self.addCleanup(shutil.rmtree, tmp_dir)
        exe = os.path.join(tmp_dir, 'hello.com')
        shutil.copyfile('/zip/.python/test/hello.com', exe)
        os.chmod(exe, 0755)
        proc = subprocess.Popen([exe], stdout=subprocess.PIPE)
        stdout, stderr = proc.communicate()
        self.assertEqual(b'hello world\n', stdout)
        self.assertEqual(0, proc.wait())


if __name__ == '__main__':
    unittest.main()
