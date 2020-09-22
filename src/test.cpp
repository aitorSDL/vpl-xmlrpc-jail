/**
 * package:		Part of vpl-jail-system
 * copyright:	Copyright (C) 2014 Juan Carlos Rodríguez-del-Pino. All rights reserved.
 * license:		GNU/GPL, see LICENSE.txt or http://www.gnu.org/licenses/gpl-3.0.html
 **/
#include <cassert>
#include <iostream>
#include <exception>
#include "util.h"
#include "cgroup.h"
#include "configuration.h"
#include <syslog.h>
#include <stdio.h>

using namespace std;
void testBase64Encode(){
	assert(Base64::encode("Hello")=="SGVsbG8="||(cerr <<Base64::encode("Hello")<<endl,0));
	assert(Base64::encode("abcde")=="YWJjZGU="||(cerr <<Base64::encode("abcde")<<endl,0));
	assert(Base64::encode("ñáÑ")=="w7HDocOR"||(cerr <<Base64::encode("ñáÑ")<<endl,0));
	assert(Base64::encode("ñáÑ=")=="w7HDocORPQ=="||(cerr <<Base64::encode("ñáÑ=")<<endl,0));
}
void testBase64Decode(){
	assert(Base64::decode("SGVsbG8=")=="Hello"||(cerr <<Base64::decode("SGVsbG8=")<<endl,0));
	assert(Base64::decode("YWJjZGU=")=="abcde"||(cerr <<Base64::decode("YWJjZGU=")<<endl,0));
	assert(Base64::decode("w7HDocOR")=="ñáÑ"||(cerr <<Base64::decode("w7HDocOR")<<endl,0));
	assert(Base64::decode("w7HDocORPQ==")=="ñáÑ="||(cerr <<Base64::decode("w7HDocORPQ==")<<endl,0));
	assert(Base64::decode(Base64::encode("RFB 003.008\n"))=="RFB 003.008\n");
	string test(256,'\0');
	unsigned char *buf=(unsigned char *)test.data();
	for(int i=0; i<256;i++)
		buf[i]=i;
	string enc=Base64::encode(test);
	string dec=Base64::decode(enc);
	assert(dec.size()==256);
	unsigned char *raw= (unsigned char *)dec.data();
	for(int i=0; i<256;i++)
		assert(i==raw[i]);

}
void testRandom(){

}
void testTrim(){

}

void testItos(){
	assert(Util::itos(0) == "0");
	assert(Util::itos(3) == "3");
	assert(Util::itos(-32780) == "-32780");
	assert(Util::itos(2038911111) == "2038911111");
	assert(Util::itos(-2038911111) == "-2038911111");
}

void testToUppercase(){
	assert(Util::toUppercase("") == "");
	assert(Util::toUppercase("a") == "A");
	assert(Util::toUppercase("a") == "A");
	assert(Util::toUppercase("á") == "á");
	assert(Util::toUppercase("ábCÑ()999  {ç") == "áBCÑ()999  {ç");
	assert(Util::toUppercase("jc\ndis") == "JC\nDIS");
	assert(Util::toUppercase("jc\\ndis") == "JC\\NDIS");
}

void testCorrectFileName(){
	assert(!Util::correctFileName("jc\ndis"));
	assert(!Util::correctFileName("jc\005dis"));
	assert(!Util::correctFileName("jc1di*s"));
	assert(!Util::correctFileName("jc1dis!"));
	assert(!Util::correctFileName("mal,c"));
	assert(!Util::correctFileName("mal:c"));
	assert(!Util::correctFileName("klsdjaf?"));
	assert(!Util::correctFileName("kl@sdjaf"));
	assert(!Util::correctFileName("kl'sdjaf"));
	assert(!Util::correctFileName("kl[sdjaf"));
	assert(!Util::correctFileName("klsdjaf^"));
	assert(!Util::correctFileName("kl\"sdjaf"));
	assert(!Util::correctFileName("klsdjaf\x60"));
	assert(!Util::correctFileName("(klsdjaf"));
	assert(!Util::correctFileName("klsd{jaf"));
	assert(!Util::correctFileName("klsdja~f"));
	assert(!Util::correctFileName("kls/djaf"));
	assert(!Util::correctFileName("\\klsdjaf.pp"));
	assert(!Util::correctFileName("nose..mal"));
	assert(!Util::correctFileName(""));
	assert(!Util::correctFileName(
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
	assert(Util::correctFileName("a"));
	assert(Util::correctFileName("kkkk"));
	assert(Util::correctFileName("a.raro"));
	assert(Util::correctFileName("normal.cvs"));
	assert(Util::correctFileName("normal.cvs.old"));
	assert(Util::correctFileName("normal-con-guiones.cvs"));
	assert(Util::correctFileName("normal_con_guiones.cvs"));
	assert(Util::correctFileName("b"));
	assert(Util::correctFileName("fichero con espacios. y varios . puntos"));
}

void testCleanPATH(){
	assert(Configuration::generateCleanPATH("","")=="");
	assert(Configuration::generateCleanPATH("/usr","")=="");
	assert(Configuration::generateCleanPATH("","/usr:/usr/bin")=="/usr:/usr/bin");
	assert(Configuration::generateCleanPATH("/usr","/bin")=="/bin");
	assert(Configuration::generateCleanPATH("/usr","/bin:/kk:/sbin")=="/bin:/sbin");
	assert(Configuration::generateCleanPATH("/usr","/bin:/kk:/sbin:/local/bin:/local/nada")=="/bin:/sbin:/local/bin");
}

void testSetCgroupFileSystem(){
	Cgroup::setBaseCgroupFileSystem("/sys/fs/cgroup");
	assert(Cgroup::getBaseCgroupFileSystem() == "/sys/fs/cgroup");
	assert(Cgroup::getBaseCgroupFileSystem() != "/sys/fs");
	Cgroup::setBaseCgroupFileSystem("/");
	assert(Cgroup::getBaseCgroupFileSystem() == "/");
	assert(Cgroup::getBaseCgroupFileSystem() != "//");
	Cgroup::setBaseCgroupFileSystem(" ");
	assert(Cgroup::getBaseCgroupFileSystem() == " ");
}

void testGetCPUAcctStat(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	map<string, int> result = cgroup.getCPUAcctStat();
	assert(result.find("user")->second == 36509);
	assert(result.find("system")->second == 3764);
}

void testGetCPUStat(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	map<string, int> result = cgroup.getCPUStat();
	assert(result.find("nr_throttled")->second == 0);
	assert(result.find("nr_periods")->second == 0);
	assert(result.find("throttled_time")->second == 0);
}

void testGetMemoryStat(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	map<string, long int> result = cgroup.getMemoryStat();
	assert(result.find("cache")->second == 1626644480);
	assert(result.find("shmem")->second == 26406912);
	assert(result.find("mapped_file")->second == 351842304);
	assert(result.find("pgfault")->second == 2448732);
	assert(result.find("hierarchical_memory_limit")->second == 9223372036854771712);
}

void testGetCPUUsage(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	long int result = cgroup.getCPUUsage();
	assert(result == 406582887060);
}

void testGetCPUNotify(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	int result = cgroup.getCPUNotify();
	assert(result == 1);
}

void testGetReleaseAgent(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	string result = cgroup.getCPUReleaseAgent();
	assert(result == "0");
}

void testGetCPUProcs(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	vector<int> pids = cgroup.getCPUProcs();
	assert(count(pids.begin(), pids.end(), 1));
	assert(count(pids.begin(), pids.end(), 4735));
	assert(count(pids.begin(), pids.end(), 4730));
	assert(count(pids.begin(), pids.end(), 8));
}

void testGetNetPrioID(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	int prioId = cgroup.getNetPrioID();
	assert(prioId == 1);
}

void testGetPIDs(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	vector<int> pids = cgroup.getPIDs();
	assert(count(pids.begin(), pids.end(), 1));
	assert(count(pids.begin(), pids.end(), 128));
	assert(count(pids.begin(), pids.end(), 3116));
	assert(count(pids.begin(), pids.end(), 1366));
}

void testGetNetPrioMap(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	map<string, int> prioMap = cgroup.getNetPrioMap();
	assert(prioMap.find("lo")->second == 0);
}

void testGetNetNotify(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	int notify = cgroup.getNetNotify();
	assert(notify == 0);
}

void testGetNetReleaseAgent(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	int releaseAgent = Util::atoi(cgroup.getNetReleaseAgent());
	assert(releaseAgent == 0);
}

void testGetNetProcs(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	vector<int> tasks = cgroup.getNetProcs();
	assert(count(tasks.begin(), tasks.end(), 1));
	assert(count(tasks.begin(), tasks.end(), 4745));
}
void testGetMemoryProcs(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	vector<int> tasks = cgroup.getMemoryProcs();
	assert(count(tasks.begin(), tasks.end(), 555));
	assert(count(tasks.begin(), tasks.end(), 7));
}

void testGetMemoryLimitInBytes(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	long int limit = cgroup.getMemoryLimitInBytes();
	assert(limit == 2147483648);
}

void testGetMemoryUsageInBytes(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	long int usage = cgroup.getMemoryUsageInBytes();
	assert(usage == 3502428160);
}

void testGetMemNotify(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	int notify = cgroup.getMemNotify();
	assert(notify == 0);
}

void testGetMemReleaseAgent(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	int releaseAgent = Util::atoi(cgroup.getMemReleaseAgent());
	assert(releaseAgent == 0);
}

void testGetMemoryOOMControl(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	map<string, int> oomControl = cgroup.getMemoryOOMControl();
	assert(oomControl.find("oom_kill_disable")->second == 0);
	assert(oomControl.find("under_oom")->second == 0);
	assert(oomControl.find("oom_kill")->second == 0);
}

void testSetNetPrioMap(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	cgroup.setNetPrioMap("eth0 1");
	map<string, int> prioMap = cgroup.getNetPrioMap();
	assert(prioMap.find("eth0")->second == 1);
}

void testSetNetNotify(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	cgroup.setNetNotify(true);
	assert(cgroup.getNetNotify() == 1);
}

void testSetNetReleaseAgent(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	cgroup.setNetReleaseAgent("0");
	assert(cgroup.getNetReleaseAgent() == "0");
}
void testSetNetProcs(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	cgroup.setNetProcs(999);
	vector<int> procs = cgroup.getNetProcs();
	assert(count(procs.begin(), procs.end(), 999));
}
void testSetCPUNotify(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	cgroup.setCPUNotify(true);
	assert(cgroup.getCPUNotify() == 1);
}
void testSetCPUReleaseAgentPath(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	cgroup.setCPUReleaseAgentPath("0");
	assert(cgroup.getCPUReleaseAgent() == "0");
}

void testSetCPUProcs(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	cgroup.setCPUProcs(999);
	vector<int> procs = cgroup.getCPUProcs();
	assert(count(procs.begin(), procs.end(), 999));
}

void testSetMemoryProcs(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	cgroup.setMemoryProcs(1);
	vector<int> procs = cgroup.getMemoryProcs();
	assert(count(procs.begin(), procs.end(), 1));
}

void testSetMemoryLimitInBytes(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	cgroup.setMemoryLimitInBytes(2147483648);
	assert(cgroup.getMemoryLimitInBytes() == 2147483648);
}

void testSetMemNotify(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	cgroup.setMemNotify(false);
	assert(cgroup.getMemNotify() == 0);
}

void testSetMemReleaseAgentPath(){
	char buff[FILENAME_MAX];
	assert(getcwd(buff, FILENAME_MAX) != NULL);
	string currentWorkingDir(buff);
	Cgroup::setBaseCgroupFileSystem(currentWorkingDir);
	Cgroup cgroup("cgroup.test");
	cgroup.setMemReleaseAgentPath("0");
	assert(cgroup.getMemReleaseAgent() == "0");
}

int main(){
	bool firstTime = true;
	while(true){
		try {
			//Test util
			testBase64Encode();
			testBase64Decode();
			testRandom();
			testTrim();
			testItos();
			testToUppercase();
			testCorrectFileName();
			//Test config
			//Test cgroup
			testSetCgroupFileSystem();
			testGetCPUAcctStat();
			testGetCPUStat();
			testGetMemoryStat();
			testGetCPUUsage();
			testGetCPUNotify();
			testGetReleaseAgent();
			testGetCPUProcs();
			testGetNetPrioID();
			testGetNetNotify();
			testGetNetReleaseAgent();
			testGetPIDs();
			testGetNetPrioMap();
			testGetNetProcs();
			testGetMemoryProcs();
			testGetMemoryLimitInBytes();
			testGetMemoryUsageInBytes();
			testGetMemNotify();
			testGetMemReleaseAgent();
			testGetMemoryOOMControl();
			testSetNetPrioMap();
			testSetNetProcs();
			testSetNetNotify();
			testSetNetReleaseAgent();
			testSetCPUNotify();
			testSetCPUReleaseAgentPath();
			testSetMemoryProcs();
			testSetMemoryLimitInBytes();
			testSetMemNotify();
			testSetMemReleaseAgentPath();
			testSetCPUProcs();
			cout << "Test Finish" << endl;
		} catch (exception &e) {
			cerr << e.what() << endl;
			if (firstTime){
				openlog("vpl-jail-system-test",LOG_PID,LOG_DAEMON);
				setlogmask(LOG_UPTO(LOG_INFO));
				firstTime = false;
				continue;
			}
			return 1;
		} catch (HttpException &e) {
			cerr << e.getMessage() << endl;
			if (firstTime){
				openlog("vpl-jail-system-test",LOG_PID,LOG_DAEMON);
				setlogmask(LOG_UPTO(LOG_INFO));
				firstTime = false;
				continue;
			}
			return 2;
		}
		break;
	}
	return 0;
}


