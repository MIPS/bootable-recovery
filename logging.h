/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _LOGGING_H
#define _LOGGING_H

#include <stddef.h>
#include <sys/types.h>

#include <string>

#include <log/log_id.h>

static constexpr int KEEP_LOG_COUNT = 10;

ssize_t logbasename(log_id_t id, char prio, const char* filename, const char* buf, size_t len,
                    void* arg);

ssize_t logrotate(log_id_t id, char prio, const char* filename, const char* buf, size_t len,
                  void* arg);

// Rename last_log -> last_log.1 -> last_log.2 -> ... -> last_log.$max.
// Similarly rename last_kmsg -> last_kmsg.1 -> ... -> last_kmsg.$max.
// Overwrite any existing last_log.$max and last_kmsg.$max.
void rotate_logs(const char* last_log_file, const char* last_kmsg_file);

// In turn fflush(3)'s, fsync(3)'s and fclose(3)'s the given stream.
void check_and_fclose(FILE* fp, const std::string& name);

void copy_log_file_to_pmsg(const std::string& source, const std::string& destination);
void copy_log_file(const std::string& source, const std::string& destination, bool append);
void copy_logs(bool modified_flash, bool has_cache);
void reset_tmplog_offset();

void save_kernel_log(const char* destination);

#endif  //_LOGGING_H
