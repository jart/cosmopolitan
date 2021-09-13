import io
import os
import unittest
import urllib.robotparser
from test import support
from http.server import BaseHTTPRequestHandler, HTTPServer
try:
    import _thread
    import threading
except ImportError:
    threading = None


class BaseRobotTest:
    robots_txt = ''
    agent = 'test_robotparser'
    good = []
    bad = []

    def setUp(self):
        lines = io.StringIO(self.robots_txt).readlines()
        self.parser = urllib.robotparser.RobotFileParser()
        self.parser.parse(lines)

    def get_agent_and_url(self, url):
        if isinstance(url, tuple):
            agent, url = url
            return agent, url
        return self.agent, url

    def test_good_urls(self):
        for url in self.good:
            agent, url = self.get_agent_and_url(url)
            with self.subTest(url=url, agent=agent):
                self.assertTrue(self.parser.can_fetch(agent, url))

    def test_bad_urls(self):
        for url in self.bad:
            agent, url = self.get_agent_and_url(url)
            with self.subTest(url=url, agent=agent):
                self.assertFalse(self.parser.can_fetch(agent, url))


class UserAgentWildcardTest(BaseRobotTest, unittest.TestCase):
    robots_txt = """\
User-agent: *
Disallow: /cyberworld/map/ # This is an infinite virtual URL space
Disallow: /tmp/ # these will soon disappear
Disallow: /foo.html
    """
    good = ['/', '/test.html']
    bad = ['/cyberworld/map/index.html', '/tmp/xxx', '/foo.html']


class CrawlDelayAndCustomAgentTest(BaseRobotTest, unittest.TestCase):
    robots_txt = """\
# robots.txt for http://www.example.com/

User-agent: *
Crawl-delay: 1
Request-rate: 3/15
Disallow: /cyberworld/map/ # This is an infinite virtual URL space

# Cybermapper knows where to go.
User-agent: cybermapper
Disallow:
    """
    good = ['/', '/test.html', ('cybermapper', '/cyberworld/map/index.html')]
    bad = ['/cyberworld/map/index.html']


class RejectAllRobotsTest(BaseRobotTest, unittest.TestCase):
    robots_txt = """\
# go away
User-agent: *
Disallow: /
    """
    good = []
    bad = ['/cyberworld/map/index.html', '/', '/tmp/']


class BaseRequestRateTest(BaseRobotTest):

    def test_request_rate(self):
        for url in self.good + self.bad:
            agent, url = self.get_agent_and_url(url)
            with self.subTest(url=url, agent=agent):
                if self.crawl_delay:
                    self.assertEqual(
                        self.parser.crawl_delay(agent), self.crawl_delay
                    )
                if self.request_rate:
                    self.assertIsInstance(
                        self.parser.request_rate(agent),
                        urllib.robotparser.RequestRate
                    )
                    self.assertEqual(
                        self.parser.request_rate(agent).requests,
                        self.request_rate.requests
                    )
                    self.assertEqual(
                        self.parser.request_rate(agent).seconds,
                        self.request_rate.seconds
                    )


class CrawlDelayAndRequestRateTest(BaseRequestRateTest, unittest.TestCase):
    robots_txt = """\
User-agent: figtree
Crawl-delay: 3
Request-rate: 9/30
Disallow: /tmp
Disallow: /a%3cd.html
Disallow: /a%2fb.html
Disallow: /%7ejoe/index.html
    """
    agent = 'figtree'
    request_rate = urllib.robotparser.RequestRate(9, 30)
    crawl_delay = 3
    good = [('figtree', '/foo.html')]
    bad = ['/tmp', '/tmp.html', '/tmp/a.html', '/a%3cd.html', '/a%3Cd.html',
           '/a%2fb.html', '/~joe/index.html']


class DifferentAgentTest(CrawlDelayAndRequestRateTest):
    agent = 'FigTree Robot libwww-perl/5.04'
    # these are not actually tested, but we still need to parse it
    # in order to accommodate the input parameters
    request_rate = None
    crawl_delay = None


class InvalidRequestRateTest(BaseRobotTest, unittest.TestCase):
    robots_txt = """\
User-agent: *
Disallow: /tmp/
Disallow: /a%3Cd.html
Disallow: /a/b.html
Disallow: /%7ejoe/index.html
Crawl-delay: 3
Request-rate: 9/banana
    """
    good = ['/tmp']
    bad = ['/tmp/', '/tmp/a.html', '/a%3cd.html', '/a%3Cd.html', '/a/b.html',
           '/%7Ejoe/index.html']
    crawl_delay = 3


class InvalidCrawlDelayTest(BaseRobotTest, unittest.TestCase):
    # From bug report #523041
    robots_txt = """\
User-Agent: *
Disallow: /.
Crawl-delay: pears
    """
    good = ['/foo.html']
    # bug report says "/" should be denied, but that is not in the RFC
    bad = []


class AnotherInvalidRequestRateTest(BaseRobotTest, unittest.TestCase):
    # also test that Allow and Diasallow works well with each other
    robots_txt = """\
User-agent: Googlebot
Allow: /folder1/myfile.html
Disallow: /folder1/
Request-rate: whale/banana
    """
    agent = 'Googlebot'
    good = ['/folder1/myfile.html']
    bad = ['/folder1/anotherfile.html']


class UserAgentOrderingTest(BaseRobotTest, unittest.TestCase):
    # the order of User-agent should be correct. note
    # that this file is incorrect because "Googlebot" is a
    # substring of "Googlebot-Mobile"
    robots_txt = """\
User-agent: Googlebot
Disallow: /

User-agent: Googlebot-Mobile
Allow: /
    """
    agent = 'Googlebot'
    bad = ['/something.jpg']


class UserAgentGoogleMobileTest(UserAgentOrderingTest):
    agent = 'Googlebot-Mobile'


class GoogleURLOrderingTest(BaseRobotTest, unittest.TestCase):
    # Google also got the order wrong. You need
    # to specify the URLs from more specific to more general
    robots_txt = """\
User-agent: Googlebot
Allow: /folder1/myfile.html
Disallow: /folder1/
    """
    agent = 'googlebot'
    good = ['/folder1/myfile.html']
    bad = ['/folder1/anotherfile.html']


class DisallowQueryStringTest(BaseRobotTest, unittest.TestCase):
    # see issue #6325 for details
    robots_txt = """\
User-agent: *
Disallow: /some/path?name=value
    """
    good = ['/some/path']
    bad = ['/some/path?name=value']


class UseFirstUserAgentWildcardTest(BaseRobotTest, unittest.TestCase):
    # obey first * entry (#4108)
    robots_txt = """\
User-agent: *
Disallow: /some/path

User-agent: *
Disallow: /another/path
    """
    good = ['/another/path']
    bad = ['/some/path']


class EmptyQueryStringTest(BaseRobotTest, unittest.TestCase):
    # normalize the URL first (#17403)
    robots_txt = """\
User-agent: *
Allow: /some/path?
Disallow: /another/path?
    """
    good = ['/some/path?']
    bad = ['/another/path?']


class DefaultEntryTest(BaseRequestRateTest, unittest.TestCase):
    robots_txt = """\
User-agent: *
Crawl-delay: 1
Request-rate: 3/15
Disallow: /cyberworld/map/
    """
    request_rate = urllib.robotparser.RequestRate(3, 15)
    crawl_delay = 1
    good = ['/', '/test.html']
    bad = ['/cyberworld/map/index.html']


class StringFormattingTest(BaseRobotTest, unittest.TestCase):
    robots_txt = """\
User-agent: *
Crawl-delay: 1
Request-rate: 3/15
Disallow: /cyberworld/map/ # This is an infinite virtual URL space

# Cybermapper knows where to go.
User-agent: cybermapper
Disallow: /some/path
    """

    expected_output = """\
User-agent: cybermapper
Disallow: /some/path

User-agent: *
Crawl-delay: 1
Request-rate: 3/15
Disallow: /cyberworld/map/

"""

    def test_string_formatting(self):
        self.assertEqual(str(self.parser), self.expected_output)


class RobotHandler(BaseHTTPRequestHandler):

    def do_GET(self):
        self.send_error(403, "Forbidden access")

    def log_message(self, format, *args):
        pass

if __name__=='__main__':
    unittest.main()
