import * as THREE from "https://esm.sh/three@0.165.0";
import { OrbitControls } from "https://esm.sh/three@0.165.0/examples/jsm/controls/OrbitControls.js";

const container = document.getElementById("laser-geometry-viewer");
const legend = document.getElementById("laser-geometry-legend");
const summary = document.getElementById("laser-geometry-summary");

function setMessage(message) {
    if (container) {
        container.textContent = message;
    }
}

function makePartMesh(part, scale) {
    const size = part.size_um;
    const center = part.center_um;
    const box = new THREE.BoxGeometry(
        size.x * scale.x,
        size.z * scale.z,
        size.y * scale.y
    );
    const material = new THREE.MeshStandardMaterial({
        color: part.color,
        transparent: true,
        opacity: 0.86,
        roughness: 0.58,
        metalness: part.material.includes("contact") ? 0.48 : 0.08,
    });
    const mesh = new THREE.Mesh(box, material);
    mesh.position.set(
        center.x * scale.x,
        center.z * scale.z,
        center.y * scale.y
    );
    mesh.userData = part;

    const edges = new THREE.EdgesGeometry(box);
    const edgeLines = new THREE.LineSegments(
        edges,
        new THREE.LineBasicMaterial({ color: 0x0a192f, transparent: true, opacity: 0.6 })
    );
    edgeLines.position.copy(mesh.position);
    return { mesh, edgeLines };
}

function addLegend(parts) {
    if (!legend) {
        return;
    }
    const unique = new Map();
    for (const part of parts) {
        if (!unique.has(part.material)) {
            unique.set(part.material, part.color);
        }
    }
    legend.innerHTML = "";
    for (const [material, color] of unique.entries()) {
        const item = document.createElement("span");
        item.className = "legend-item";
        item.innerHTML = `<span class="legend-swatch" style="background:${color}"></span>${material}`;
        legend.appendChild(item);
    }
}

function addSummary(data) {
    if (!summary) {
        return;
    }
    const bounds = data.metadata.physical_bounds_um;
    const scale = data.metadata.visual_scale;
    summary.innerHTML = `
        <strong>${data.metadata.name}</strong><br>
        Physical size: ${bounds.length.toFixed(1)} um x ${bounds.width.toFixed(1)} um x ${bounds.height.toFixed(2)} um.
        Visualization scale: x=${scale.x}, y=${scale.y}, z=${scale.z}.
        Parts: ${data.parts.length}.
    `;
}

async function main() {
    if (!container) {
        return;
    }

    const response = await fetch("telecom_laser_geometry.json");
    if (!response.ok) {
        throw new Error("Unable to load telecom_laser_geometry.json");
    }
    const data = await response.json();
    const scale = data.metadata.visual_scale;

    container.textContent = "";
    const scene = new THREE.Scene();
    scene.background = new THREE.Color(0x0a192f);

    const camera = new THREE.PerspectiveCamera(35, container.clientWidth / container.clientHeight, 0.1, 10000);
    camera.position.set(520, 520, 420);

    const renderer = new THREE.WebGLRenderer({ antialias: true });
    renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));
    renderer.setSize(container.clientWidth, container.clientHeight);
    container.appendChild(renderer.domElement);

    const controls = new OrbitControls(camera, renderer.domElement);
    controls.enableDamping = true;
    controls.dampingFactor = 0.08;

    scene.add(new THREE.AmbientLight(0xffffff, 0.58));
    const keyLight = new THREE.DirectionalLight(0xffffff, 1.1);
    keyLight.position.set(300, 500, 700);
    scene.add(keyLight);
    const fillLight = new THREE.DirectionalLight(0x64ffda, 0.45);
    fillLight.position.set(-600, 200, -300);
    scene.add(fillLight);

    const group = new THREE.Group();
    for (const part of data.parts) {
        const { mesh, edgeLines } = makePartMesh(part, scale);
        group.add(mesh);
        group.add(edgeLines);
    }
    scene.add(group);

    const box = new THREE.Box3().setFromObject(group);
    const center = box.getCenter(new THREE.Vector3());
    group.position.sub(center);
    controls.target.set(0, 0, 0);

    const axes = new THREE.AxesHelper(170);
    axes.position.set(-210, -320, -170);
    scene.add(axes);

    addLegend(data.parts);
    addSummary(data);

    function resize() {
        const width = container.clientWidth;
        const height = container.clientHeight;
        camera.aspect = width / height;
        camera.updateProjectionMatrix();
        renderer.setSize(width, height);
    }

    window.addEventListener("resize", resize);

    function animate() {
        controls.update();
        renderer.render(scene, camera);
        requestAnimationFrame(animate);
    }
    animate();
}

main().catch((error) => {
    console.error(error);
    setMessage("The 3D model viewer could not be loaded. The JSON and OBJ geometry files are available below.");
});
