/***************************************************************************
 *            test_buffer.cpp
 *
 *  Copyright  2008-20  Luca Geretti
 *
 ****************************************************************************/

/*
 *  This file is part of Ariadne.
 *
 *  Ariadne is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Ariadne is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Ariadne.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <thread>
#include "concurrency/buffer.hpp"
#include "../test.hpp"

using namespace Ariadne;

class TestBuffer {
  public:

    void test_single_buffer() {
        Buffer<unsigned int> buffer(2);
        buffer.push(4);
        buffer.push(2);
        auto o1 = buffer.pull();
        auto o2 = buffer.pull();
        ARIADNE_TEST_EQUALS(o1,4);
        ARIADNE_TEST_EQUALS(o2,2);
    }

    void test_io_buffer() {
        Buffer<unsigned int> ib(2);
        Buffer<unsigned int> ob(2);

        std::thread thread([&ib,&ob]() {
            while (true) {
                try {
                    auto i = ib.pull();
                    ob.push(i);
                } catch (BufferStoppedConsumingException& e) {
                    break;
                }
            }
        });
        ib.push(4);
        ib.push(2);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ARIADNE_TEST_EQUALS(ib.size(),0);
        ARIADNE_TEST_EQUALS(ob.size(),2);
        auto o1 = ob.pull();
        ARIADNE_TEST_EQUALS(ob.size(),1);
        ARIADNE_TEST_EQUALS(o1,4);
        auto o2 = ob.pull();
        ARIADNE_TEST_EQUALS(ob.size(),0);
        ARIADNE_TEST_EQUALS(o2,2);
        ib.stop_consuming();
        thread.join();
    }

    void test() {
        ARIADNE_TEST_CALL(test_single_buffer());
        ARIADNE_TEST_CALL(test_io_buffer());
    }
};

int main() {
    TestBuffer().test();
    return ARIADNE_TEST_FAILURES;
}
