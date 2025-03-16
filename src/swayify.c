#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wayland-client.h>

//// UTILS

void expects(bool instance, const char* msg) {
    if (!instance) {
        fprintf(stderr, "Expected: %s\n", msg);
        exit(1);
    }
}

//// STRUCTS

typedef struct sy_globals {
    struct wl_compositor *compositor;
    struct wl_shm *shm;
} sy_globals;

bool is_valid(sy_globals *globals) {
    return globals->compositor && globals->shm;
}
sy_globals init_globals() {
    return (sy_globals) {0};
}

typedef struct sy_state {
    sy_globals globals;
} sy_state;

sy_state init_state() {
    return (sy_state) {0};
}

//// MAIN

static void registry_handle_global(
    void *data, 
    struct wl_registry *registry,
    uint32_t name, 
    const char *interface, 
    uint32_t version
) {
    sy_state *state = data;
    if (!strcmp(interface, "wl_compositor")) {
        state->globals.compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 4);
    } else if (!strcmp(interface, "wl_shm")) {
        state->globals.shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    }
}

static void registry_handle_global_remove(
    void *data,
    struct wl_registry *registry,
    uint32_t name
) {
	// This space deliberately left blank
}

static const struct wl_registry_listener registry_listener = {
	.global = registry_handle_global,
	.global_remove = registry_handle_global_remove,
};

int main(/* int argc, char *argv[] */) {
    struct wl_display *display = wl_display_connect(NULL);
    expects(display, "Failed to connect to Wayland display!");
    struct wl_registry *registry = wl_display_get_registry(display);
    expects(registry, "Failed to get Wayland registry!");
    
    // Allows to handle the global objects given by the server
    sy_state state = init_state();
    wl_registry_add_listener(registry, &registry_listener, &state);
    wl_display_dispatch(display);

    struct wl_surface *surface = wl_compositor_create_surface(state.globals.compositor);
 
    while (wl_display_dispatch(display) != -1) {
        /* This space deliberately left blank */
    }
        
    wl_display_disconnect(display);
    return 0;
}