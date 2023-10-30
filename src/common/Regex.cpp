#include "Regex.h"

using namespace std;

// General
const regex Regex::split(
    R"(^\s*(.*?);\s*(.*?)\s*$)",
    regex::icase | regex::optimize
);

// Common
const regex Regex::ack(
    R"(^\s*ACK;\s*(.*?)\s*)",
    regex::icase | regex::optimize
);
const regex Regex::nack(
    R"(^\s*NACK;\s*(.*?)\s*)",
    regex::icase | regex::optimize
);
const regex Regex::value(
    R"(^\s*VALUE;\s*(.*?)\s*)",
    regex::icase | regex::optimize
);

// Server
const regex Regex::fetch(
    R"(\s*FETCH;\s*k:\"(.*?)\"\s*)",
    regex::icase | regex::optimize
);
const regex Regex::fetchedValue(
    R"(\s*FETCHED_VALUE;\s*k:\"(.*?)\",\s*v:\"(.*?)\",\s*ts:(\d*)\s*)",
    regex::icase | regex::optimize
);
const regex Regex::fetchedValueNull(
    R"(\s*FETCHED_VALUE;\s*k:\"(.*?)\",\s*v:null\s*)",
    regex::icase | regex::optimize
);
const regex Regex::get(
    R"(\s*GET;\s*k:\"(.*?)\"\s*)",
    regex::icase | regex::optimize
);
const regex Regex::globalAbort(
    R"(\s*GLOBAL_ABORT;\s*tId:(\d*),\s*k:\"(.*?)\"\s*)",
    regex::icase | regex::optimize
);
const regex Regex::globalCommit(
    R"(\s*GLOBAL_COMMIT;\s*tId:(\d*),\s*k:\"(.*?)\",\s*v:\"(.*?)\",\s*ts:(\d*)\s*)",
    regex::icase | regex::optimize
);
const regex Regex::prepare(
    R"(\s*PREPARE;\s*tId:(\d*),\s*k:\"(.*?)\",\s*v:\"(.*?)\"\s*)",
    regex::icase | regex::optimize
);
const regex Regex::put(
    R"(\s*PUT;\s*k:\"(.*?)\",\s*v:\"(.*?)\"\s*)",
    regex::icase | regex::optimize
);
const regex Regex::repair(
    R"(\s*REPAIR;\s*k:\"(.*?)\",\s*v:\"(.*?)\",\s*ts:(\d*)\s*)",
    regex::icase | regex::optimize
);
const regex Regex::sync(
    R"(\(k:\"(.*?)\",\s*ts:(\d*)\),\s*(.*?))",
    regex::icase | regex::optimize
);
const regex Regex::syncEnd(
    R"(\(k:\"(.*?)\",\s*ts:(\d*)\))",
    regex::icase | regex::optimize
);
const regex Regex::syncStart(
    R"(^\s*SYNC;\s*(.*?)\s*)",
    regex::icase | regex::optimize
);
const regex Regex::update(
    R"(\(k:\"(.*?)\",\s*v:\"(.*?)\",\s*ts:(\d*)\),\s*(.*?))",
    regex::icase | regex::optimize
);
const regex Regex::updateEnd(
    R"(\(k:\"(.*?)\",\s*v:\"(.*?)\",\s*ts:(\d*)\))",
    regex::icase | regex::optimize
);
const regex Regex::updateStart(
    R"(^\s*UPDATE;\s*(.*?)\s*)",
    regex::icase | regex::optimize
);
const regex Regex::voteAbort(
    R"(\s*VOTE_ABORT;\s*tId:(\d*),\s*k:\"(.*?)\"\s*)",
    regex::icase | regex::optimize
);
const regex Regex::voteCommit(
    R"(\s*VOTE_COMMIT;\s*tId:(\d*),\s*k:\"(.*?)\",\s*ts:(\d*)\s*)",
    regex::icase | regex::optimize
);
