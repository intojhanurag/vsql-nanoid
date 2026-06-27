// Copyright (c) 2026 VillageSQL Contributors
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <https://www.gnu.org/licenses/>.

#include <villagesql/vsql.h>

#include <string>
#include <string_view>

#include <openssl/rand.h>

using namespace vsql;

static constexpr int kDefaultSize = 21;
static constexpr std::string_view kDefaultAlphabet =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-";

// Generates a nanoid string of the given length using the given alphabet.
// Uses rejection sampling to avoid modulo bias: for an alphabet of size N,
// we compute a mask that is the largest (2^k - 1) >= N, generate random
// bytes, mask each byte, and keep it only if it falls within the alphabet.
static bool generate_nanoid(std::string &out, int size,
                            std::string_view alphabet) {
  out.clear();
  out.reserve(static_cast<size_t>(size));

  int alpha_len = static_cast<int>(alphabet.size());
  int mask = 1;
  while (mask < alpha_len) mask <<= 1;
  mask -= 1;

  // Generate random bytes in batches to reduce RAND_bytes calls.
  // step estimates how many random bytes we need per character on average,
  // accounting for rejection. Formula from the original nanoid source.
  int step = (int)(1.6 * static_cast<double>(mask * size) / alpha_len) + 1;
  unsigned char buf[256];

  while (static_cast<int>(out.size()) < size) {
    int batch = step < 256 ? step : 256;
    if (RAND_bytes(buf, batch) != 1) return false;

    for (int i = 0; i < batch && static_cast<int>(out.size()) < size; i++) {
      int idx = buf[i] & mask;
      if (idx < alpha_len) {
        out += alphabet[static_cast<size_t>(idx)];
      }
    }
  }
  return true;
}

// nanoid() -> TEXT
// Returns a 21-character random ID using the default alphabet.
void vdf_nanoid(StringResult out) {
  std::string id;
  if (!generate_nanoid(id, kDefaultSize, kDefaultAlphabet)) {
    out.error("failed to generate random bytes");
    return;
  }
  out.set(std::string_view(id));
}

// nanoid_with(size INT, [alphabet TEXT]) -> TEXT
// Prerun validates argument count and types.
void nanoid_with_prerun(PrerunArgs args, PrerunResult out) {
  if (args.size() < 1 || args.size() > 2) {
    out.error("nanoid_with requires 1 or 2 arguments: (size INT [, alphabet TEXT])");
    return;
  }
  // NULL literals arrive as VEF_TYPE_STRING in prerun, so accept both INT
  // and STRING for the first argument. Runtime null check happens in the VDF.
  if (!args.type_at(0).is_int() && !args.type_at(0).is_str()) {
    out.error("nanoid_with: first argument must be INT");
    return;
  }
  out.request_buffer_size(1024);
}

void vdf_nanoid_with(VarArgs args, StringResult out) {
  auto size_arg = args[0];
  if (size_arg.is_null()) {
    out.set_null();
    return;
  }

  int size = static_cast<int>(size_arg.as_int());
  if (size < 1 || size > 1024) {
    out.error("size must be between 1 and 1024");
    return;
  }

  std::string_view alphabet = kDefaultAlphabet;
  if (args.size() == 2) {
    auto alpha_arg = args[1];
    if (alpha_arg.is_null()) {
      out.set_null();
      return;
    }
    alphabet = alpha_arg.as_str();
    if (alphabet.empty() || alphabet.size() > 255) {
      out.error("alphabet must be 1-255 characters");
      return;
    }
  }

  std::string id;
  if (!generate_nanoid(id, size, alphabet)) {
    out.error("failed to generate random bytes");
    return;
  }
  out.set(std::string_view(id));
}

VEF_GENERATE_ENTRY_POINTS(
    make_extension()
        .func(make_func<&vdf_nanoid>("nanoid")
                  .returns(STRING)
                  .no_params()
                  .buffer_size(64)
                  .build())
        .func(make_func<&vdf_nanoid_with>("nanoid_with")
                  .returns(STRING)
                  .varargs()
                  .prerun<&nanoid_with_prerun>()
                  .buffer_size(1024)
                  .build()))
