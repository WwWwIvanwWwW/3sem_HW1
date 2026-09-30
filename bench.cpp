#include "ShrdPtr.hpp"
#include "UnqPtr.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

using Clock = std::chrono::high_resolution_clock;

double now_ms()
{
	return std::chrono::duration<double, std::milli>(
			   Clock::now().time_since_epoch())
		.count();
}

std::size_t rss_kb()
{
	std::ifstream f("/proc/self/status");
	std::string line;
	while (std::getline(f, line))
		if (line.rfind("VmHWM:", 0) == 0)
			return std::stoul(line.substr(6));
	return 0;
}

struct Result {
	double create_ms;
	double destroy_ms;
	std::size_t rss_kb;
};

void run_impl(const std::string &impl, std::size_t n, Result &r)
{
	std::size_t m0 = rss_kb();
	double t0, t1, t2, t3;

	if (impl == "raw") {
		std::vector<int *> v;
		v.reserve(n);
		t0 = now_ms();
		for (std::size_t i = 0; i < n; ++i)
			v.push_back(new int(1));
		t1 = now_ms();
		t2 = now_ms();
		for (auto p : v)
			delete p;
		t3 = now_ms();
	} else if (impl == "UnqPtr") {
		std::vector<UnqPtr<int>> v;
		v.reserve(n);
		t0 = now_ms();
		for (std::size_t i = 0; i < n; ++i)
			v.emplace_back(new int(1));
		t1 = now_ms();
		t2 = now_ms();
		v.clear();
		t3 = now_ms();
	} else if (impl == "ShrdPtr") {
		std::vector<ShrdPtr<int>> v;
		v.reserve(n);
		t0 = now_ms();
		for (std::size_t i = 0; i < n; ++i)
			v.emplace_back(new int(1));
		t1 = now_ms();
		t2 = now_ms();
		v.clear();
		t3 = now_ms();
	} else if (impl == "std::unique_ptr") {
		std::vector<std::unique_ptr<int>> v;
		v.reserve(n);
		t0 = now_ms();
		for (std::size_t i = 0; i < n; ++i)
			v.emplace_back(new int(1));
		t1 = now_ms();
		t2 = now_ms();
		v.clear();
		t3 = now_ms();
	} else if (impl == "std::shared_ptr") {
		std::vector<std::shared_ptr<int>> v;
		v.reserve(n);
		t0 = now_ms();
		for (std::size_t i = 0; i < n; ++i)
			v.emplace_back(new int(1));
		t1 = now_ms();
		t2 = now_ms();
		v.clear();
		t3 = now_ms();
	} else {
		std::fprintf(stderr, "unknown impl: %s\n", impl.c_str());
		std::exit(1);
	}

	std::size_t m1 = rss_kb();
	r.create_ms = t1 - t0;
	r.destroy_ms = t3 - t2;
	r.rss_kb = m1 > m0 ? m1 - m0 : 0;
}

Result measure_in_child(const std::string &impl, std::size_t n)
{
	int fd[2];
	if (pipe(fd) != 0) {
		std::perror("pipe");
		std::exit(1);
	}

	pid_t pid = fork();
	if (pid < 0) {
		std::perror("fork");
		std::exit(1);
	}

	if (pid == 0) {
		close(fd[0]);
		Result r{};
		run_impl(impl, n, r);
		char buf[128];
		int len = std::snprintf(buf, sizeof(buf), "%.6f %.6f %zu\n",
								r.create_ms, r.destroy_ms, r.rss_kb);
		ssize_t ignored = write(fd[1], buf, len);
		(void)ignored;
		close(fd[1]);
		_exit(0);
	}

	close(fd[1]);
	char buf[128] = {0};
	ssize_t got = read(fd[0], buf, sizeof(buf) - 1);
	(void)got;
	close(fd[0]);
	waitpid(pid, nullptr, 0);

	Result r{0, 0, 0};
	std::sscanf(buf, "%lf %lf %zu", &r.create_ms, &r.destroy_ms, &r.rss_kb);
	return r;
}

int main()
{
	const std::vector<std::string> impls = {
		"raw", "UnqPtr", "ShrdPtr", "std::unique_ptr", "std::shared_ptr"};
	const std::vector<std::size_t> sizes = {10,	   100,	   1000,
											10000, 100000, 1000000};

	std::vector<std::vector<Result>> R(sizes.size(),
									   std::vector<Result>(impls.size()));

	std::printf("%-16s %10s %12s %12s %10s\n", "impl", "n", "create_ms",
				"destroy_ms", "rss_kb");

	for (std::size_t i = 0; i < sizes.size(); ++i) {
		for (std::size_t j = 0; j < impls.size(); ++j) {
			R[i][j] = measure_in_child(impls[j], sizes[i]);
			std::printf("%-16s %10zu %12.3f %12.3f %10zu\n", impls[j].c_str(),
						sizes[i], R[i][j].create_ms, R[i][j].destroy_ms,
						R[i][j].rss_kb);
		}
		std::printf("\n");
	}

	auto write_csv = [&](const char *path, auto get) {
		std::ofstream f(path);
		f << "n";
		for (auto &name : impls)
			f << ',' << name;
		f << '\n';
		for (std::size_t i = 0; i < sizes.size(); ++i) {
			f << sizes[i];
			for (std::size_t j = 0; j < impls.size(); ++j)
				f << ',' << get(R[i][j]);
			f << '\n';
		}
	};

	write_csv("create.csv", [](const Result &r) { return r.create_ms; });
	write_csv("destroy.csv", [](const Result &r) { return r.destroy_ms; });
	write_csv("rss.csv", [](const Result &r) { return r.rss_kb; });

	return 0;
}