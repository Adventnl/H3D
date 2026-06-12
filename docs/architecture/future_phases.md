# Future Phases

Forge3D is planned as ten large phases. Each phase produces a usable,
tested increment; no phase starts before the previous one's interfaces are
stable enough to build on.

## Phase 1 — Foundation, build system, core runtime *(this repository state)*

Repository structure, CMake/Ninja/C++23 build, logging, assertions, math,
memory layer, file/path/VFS, time utilities, thread pool, job system,
profiler, unit-test and benchmark frameworks, CI.

## Phase 2 — Application shell, UI, workspaces, operators

Window manager and native windows, input system, dockable/splittable editor
UI, workspaces, panels, menus, toolbars, shortcuts and keymaps, preferences,
themes, command search, modal tools, the operator system, and undo/redo
transactions.

## Phase 3 — Scene database, dependency graph, file format

Data-block system with stable IDs, scenes, objects, collections, view
layers, transforms and parenting, the dependency graph with dirty
propagation and evaluated state, the native file format, save/load,
autosave, crash recovery, and file version migration.

## Phase 4 — Viewport, GPU backend, object mode, selection

GPU abstraction with a Vulkan backend, render graph, viewport camera and
navigation, grid/axes/overlays, object drawing, selection and picking,
move/rotate/scale with gizmos, snapping, pivot and orientation modes, and
object primitives.

## Phase 5 — Geometry, modeling, UVs, modifiers

Mesh data structure (vertex/edge/face/loop, n-gons), normals and tangents,
UV layers and generic attributes, edit mode with mesh selection, the
modeling toolset (extrude, inset, bevel, knife, loop cut, bridge, fill,
merge, weld, dissolve, booleans, subdivision, remeshing), the modifier
stack, and the UV editor with unwrapping, packing and UDIM support.

## Phase 6 — Materials, nodes, rendering

Material and texture data-blocks, image loading, the shader node graph and
node editor, a principled shader, procedural textures, a workbench renderer,
a real-time renderer (shadows, AO, reflections, refractions, volumetrics),
CPU and GPU path tracers with BVH, denoising, baking, and command-line
rendering.

## Phase 7 — Animation, rigging, character tools

Timeline, keyframes and f-curves, Graph Editor and Dope Sheet, actions and
NLA, drivers and constraints, motion paths, armatures and bones, pose mode,
IK/FK, skinning and weight painting, shape keys, a pose library, a rig
generator, and retargeting.

## Phase 8 — Geometry nodes, sculpt/paint, Grease Pencil

A generic node execution engine, geometry sets, the attribute and field
systems, the Geometry Nodes editor with mesh/curve/point/instance/volume
nodes, node groups, simulation zones and node baking; sculpting with a brush
engine, masks, face sets, voxel remesh, dynamic topology and multires;
texture/vertex paint; and Grease Pencil-style 2D/3D drawing with onion
skinning, Line Art and modifiers.

## Phase 9 — Simulation, compositor, VFX, video sequencer

The simulation framework with caching/baking, force fields and collisions,
rigid bodies, cloth, particles, hair, soft bodies, fluids, smoke/fire and
ocean; the compositor with render layers and color/filter/mask nodes and
Cryptomatte-like IDs; motion tracking with camera/object/plane solving, lens
distortion and stabilization; rotoscoping masks; and the video sequencer
with strips, effects, transitions, retiming and an export pipeline.

## Phase 10 — Assets, import/export, Python, add-ons, production hardening

Asset browser with catalogs, thumbnails, metadata and search; material,
brush, pose and node-group assets; linked libraries and overrides; OBJ, STL,
PLY, glTF, FBX, USD, Alembic, BVH, SVG, OpenEXR and image/video/audio
formats; embedded Python with scene/operator/UI/node APIs; the add-on loader
and extension system; massive-scene performance, GPU device-loss and file
corruption recovery; regression testing, performance dashboards, crash
analytics, installer/updater, documentation and the LTS release process.
