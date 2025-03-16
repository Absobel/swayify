#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wayland-client.h>
#include <wayland-client-protocol.h>

//// UTILS

void expects(bool instance, const char* msg) {
    if (!instance) {
        fprintf(stderr, "Expected: %s\n", msg);
        exit(1);
    }
}

//// STRUCTS

typedef struct sy_output {
    struct wl_output *output;
    // add relevant output data here
} sy_output;

typedef struct sy_outputs {
    sy_output *arr;
    size_t len;
} sy_outputs;

typedef struct sy_globals {
    struct wl_compositor *compositor;
    struct wl_shm *shm;
    sy_outputs outputs;
} sy_globals;

bool is_valid(sy_globals *globals) {
    return globals->compositor && globals->shm && globals->outputs.arr;
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

//// LISTENERS

/// Output

static void output_geometry_handler(
    void *data,
    struct wl_output *wl_output,
    int32_t x,
    int32_t y,
    int32_t physical_width,
    int32_t physical_height,
    int32_t subpixel,
    const char *make,
    const char *model,
    int32_t transform
) {}

static void output_mode_handler(
    void *data,
    struct wl_output *wl_output,
    uint32_t flags,
    int32_t width,
    int32_t height,
    int32_t refresh
) {
    printf("Output mode:\n");
    printf("  flags: %d\n", flags);
    printf("  width: %d\n", width);
    printf("  height: %d\n", height);
    printf("  refresh: %d\n", refresh);
}

static void output_done_handler(
    void *data,
    struct wl_output *wl_output
) {}

static void output_scale_handler(
    void *data,
    struct wl_output *wl_output,
    int32_t factor
) {}

static void output_name_handler(
    void *data,
    struct wl_output *wl_output,
    const char *name
) {
    printf("Output name: %s\n", name);
}

static void output_description_handler(
    void *data,
    struct wl_output *wl_output,
    const char *description
) {}

// Define the complete wl_output_listener
static const struct wl_output_listener output_listener = {
    .geometry = output_geometry_handler,
    .mode = output_mode_handler,
    .done = output_done_handler,
    .scale = output_scale_handler,
    .name = output_name_handler,
    .description = output_description_handler
};

/// Globals

static void registry_global_handler(
    void *data, 
    struct wl_registry *registry,
    uint32_t name, 
    const char *interface, 
    uint32_t version
) {
    sy_state *state = data;
    if (!strcmp(interface, "wl_compositor")) {
        state->globals.compositor = wl_registry_bind(registry, name, &wl_compositor_interface, version);
    } else if (!strcmp(interface, "wl_shm")) {
        state->globals.shm = wl_registry_bind(registry, name, &wl_shm_interface, version);
    } else if (!strcmp(interface, "wl_output")) {
        state->globals.outputs.arr = realloc(state->globals.outputs.arr, sizeof(struct sy_output) * (state->globals.outputs.len + 1));
        state->globals.outputs.arr[state->globals.outputs.len].output = wl_registry_bind(registry, name, &wl_output_interface, version);
        state->globals.outputs.len++;
        wl_output_add_listener(state->globals.outputs.arr[state->globals.outputs.len - 1].output, &output_listener, NULL);
    }
}

static void registry_global_remove(
    void *data,
    struct wl_registry *registry,
    uint32_t name
) {
	// This space deliberately left blank
}

static const struct wl_registry_listener registry_listener = {
	.global = registry_global_handler,
	.global_remove = registry_global_remove,
};

//// MAIN

int main(/* int argc, char *argv[] */) {
    //// INITIALIZATION
    struct wl_display *display = wl_display_connect(NULL);
    expects(display, "correctly initialized display");
    struct wl_registry *registry = wl_display_get_registry(display);
    expects(registry, "correctly initialized registry");
    
    //// GLOBALS
    sy_state state = init_state();
    wl_registry_add_listener(registry, &registry_listener, &state);
    wl_display_roundtrip(display);
    wl_display_dispatch(display);
    expects(is_valid(&state.globals), "globals initialized");

    //// ACTUAL PROGRAM

    struct wl_surface *surface = wl_compositor_create_surface(state.globals.compositor);
        
    wl_display_disconnect(display);
    return 0;
}