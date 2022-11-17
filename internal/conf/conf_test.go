package conf

import (
	"bytes"
	"io/ioutil"
	"reflect"
	"testing"
)

func TestCommentsAreStripped(t *testing.T) {
	cases := []struct {
		confFile     string
		confFileWant string
	}{
		{"testdata/conf1.conf", "testdata/conf1_want.conf"},
		{"testdata/conf2.conf", "testdata/conf2_want.conf"},
	}

	for _, c := range cases {
		file, err := openAndStripComments(c.confFile)
		if err != nil {
			t.Errorf("error reading conf file %s", err)
		}
		want, err := ioutil.ReadFile(c.confFileWant)
		if err != nil {
			t.Errorf("error reading conf_want file %s", err)
		}
		if !bytes.Equal(file, want) {
			t.Errorf("bytes in file are not equal")
		}
	}
}

func TestIncludedFilesAreParsed(t *testing.T) {
	t.Skipf("todo")
}

func TestNoSyntaxErrorsAreFound(t *testing.T) {
	t.Skipf("todo")
}

func TestSyntaxErrorsAreFound(t *testing.T) {
	t.Skipf("todo")
}

func TestServerConfIsBuilt(t *testing.T) {
	wantConf := Conf{
		ServerName: "localhost",
		Port:       20,
		Root:       "/home/jt/ftpd_test",
		ErrorLog:   "/etc/log/ftpd/errors.log",
		AccessLog:  "/etc/log/ftpd/access.log",
		Users: []User{
			{
				Username: "test1",
				Password: "test1",
				Root:     "/test1",
			},
			{
				Username: "test2",
				Password: "test2",
				Root:     "/test2",
			},
			{
				Username: "test3",
				Password: "test3",
				Root:     "/test3",
			},
		},
	}
	cases := []struct {
		confFile string
		want     Conf
	}{
		{"testdata/conf3.conf", wantConf},
	}

	for _, c := range cases {
		file, err := ioutil.ReadFile(c.confFile)
		if err != nil {
			t.Errorf("%s", err)
		}
		conf, err := buildServerConf(file, "")
		if err != nil {
			t.Errorf("%s", err)
		}
		if !reflect.DeepEqual(*conf, c.want) {
			t.Errorf("the configurations are not equal")
		}
	}
}
