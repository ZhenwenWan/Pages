const frame = document.getElementById("profile2d-viewer");
const content = document.getElementById("profile2d-content");
const zoomInButton = document.getElementById("profile2d-zoom-in");
const zoomOutButton = document.getElementById("profile2d-zoom-out");
const resetButton = document.getElementById("profile2d-reset");

let scale = 1;
let panX = 0;
let panY = 0;
let dragging = false;
let lastX = 0;
let lastY = 0;

function applyTransform() {
    if (!content) {
        return;
    }
    content.style.transform = `translate(${panX}px, ${panY}px) scale(${scale})`;
}

function clampScale(nextScale) {
    return Math.min(6, Math.max(0.5, nextScale));
}

function zoomAt(delta, originX, originY) {
    const oldScale = scale;
    scale = clampScale(scale * delta);
    const ratio = scale / oldScale;
    panX = originX - (originX - panX) * ratio;
    panY = originY - (originY - panY) * ratio;
    applyTransform();
}

if (frame && content) {
    frame.addEventListener("wheel", (event) => {
        event.preventDefault();
        const rect = frame.getBoundingClientRect();
        const factor = event.deltaY < 0 ? 1.12 : 0.89;
        zoomAt(factor, event.clientX - rect.left, event.clientY - rect.top);
    }, { passive: false });

    frame.addEventListener("pointerdown", (event) => {
        dragging = true;
        lastX = event.clientX;
        lastY = event.clientY;
        frame.setPointerCapture(event.pointerId);
        frame.classList.add("is-dragging");
    });

    frame.addEventListener("pointermove", (event) => {
        if (!dragging) {
            return;
        }
        panX += event.clientX - lastX;
        panY += event.clientY - lastY;
        lastX = event.clientX;
        lastY = event.clientY;
        applyTransform();
    });

    frame.addEventListener("pointerup", (event) => {
        dragging = false;
        frame.releasePointerCapture(event.pointerId);
        frame.classList.remove("is-dragging");
    });

    frame.addEventListener("pointercancel", () => {
        dragging = false;
        frame.classList.remove("is-dragging");
    });
}

if (zoomInButton && frame) {
    zoomInButton.addEventListener("click", () => zoomAt(1.2, frame.clientWidth / 2, frame.clientHeight / 2));
}

if (zoomOutButton && frame) {
    zoomOutButton.addEventListener("click", () => zoomAt(0.84, frame.clientWidth / 2, frame.clientHeight / 2));
}

if (resetButton) {
    resetButton.addEventListener("click", () => {
        scale = 1;
        panX = 0;
        panY = 0;
        applyTransform();
    });
}
