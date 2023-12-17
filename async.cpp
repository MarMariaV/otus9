#include "async.h"
#include "bulk_class.h"

namespace async {

handle_t connect(std::size_t bulk) {
    return new Bulk(bulk);
}

void receive(handle_t handle, const char *data, std::size_t size) {
    handle->setBulk(std::string(data), size);
}

void disconnect(handle_t handle) {
    handle->exit();
    delete handle;
}

}
