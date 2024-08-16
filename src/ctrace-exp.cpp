
#include <cpptrace/cpptrace.hpp>
#include <map>

#include <time.h>

#if 1
class TraceEntry {
public:
	TraceEntry() : count(0) { };

	uint64_t count;
	std::vector<std::uintptr_t> stack;
	std::string backTrace;
};

std::map<std::vector<uintptr_t>, TraceEntry> entries;
uint64_t lastTS, iVal;

extern "C" {

uint64_t getTS() {
	struct timespec ts;
	uint64_t ticks;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	ticks = ts.tv_nsec;
	ticks += ts.tv_sec * 1000000000;

	return ticks;
}

void initTrace(uint64_t ival) {
	iVal = ival;
	lastTS = getTS();
}

void trace() {
	uint64_t now = getTS();
	uint64_t count = 0;

	while (now > (lastTS + iVal)) {
		count++;
		lastTS += iVal;
	}

	if (count == 0) {
		return;
	}

	cpptrace::raw_trace rt = cpptrace::generate_raw_trace();
	
	auto it = entries.find(rt.frames);

	if (it != entries.end()) {
		it->second.count += count;
		return;
	}

	TraceEntry te;
	te.count = count;
	te.stack = rt.frames;
	//te.backTrace = cpptrace::generate_trace().to_string();
	cpptrace::stacktrace st = cpptrace::generate_trace();
	auto sf = st.frames.rbegin();
	te.backTrace = sf->symbol;
	for (++sf; sf != st.frames.rend(); ++sf) {
		if (sf->symbol.empty()) continue;
		te.backTrace += ";";
		te.backTrace += sf->symbol;
	}

	entries[rt.frames] = te;

	//cpptrace::generate_trace().print();
}

void endTrace() {
	const char *fn = getenv("BT_FILE");
	FILE *fd = stdout;

	if (fn != NULL) fd = fopen(fn, "w");

	for (auto it = entries.begin(); it != entries.end(); ++it) {
		fprintf(fd, "%s %lu\n", it->second.backTrace.c_str(), it->second.count);
	}
	fclose(fd);
}

}

#else

extern "C" {
	void trace() { }
	void initTrace(uint64_t ival) { }
	void endTrace() { };
}

#endif
