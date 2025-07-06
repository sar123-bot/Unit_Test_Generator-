#!/usr/bin/env python3
import os
import subprocess
from llama_cpp import Llama

llm = Llama(model_path="capybarahermes-2.5-mistral-7b.Q4_K_M.gguf.1" ,_n_ctx=4096) 


# ── CONFIG ─────────────────────────────────────────────────────────────
MODEL_PATH = "capybarahermes-2.5-mistral-7b.Q4_K_M.gguf.1"
INPUT_DIR = "inputs"
TEST_DIR = "tests"
LOG_DIR = "logs"
PROMPTS = {
    "generate": "prompts/prompt1.yaml",
    "refine":   "prompts/prompt2.yaml",
    "fix":      "prompts/prompt3.yaml",
}
BUILD_SCRIPT = "./build_and_test.sh"
# ───────────────────────────────────────────────────────────────────────

# Initialize LLaMA
llm = Llama(model_path=MODEL_PATH, n_ctx=4096)


def load_file(path):
    with open(path, "r", encoding="utf-8") as f:
        return f.read()

def save_file(path, content):
    with open(path, "w", encoding="utf-8") as f:
        f.write(content)

def call_llm(prompt_yaml, code_files):
    """
    Passes code_files + prompt_yaml to LLaMA and returns generated text.
    """
    # Combine prompt + code
    prompt = load_file(prompt_yaml)
    for cf in code_files:
        prompt += f"\n\n// ===== File: {os.path.basename(cf)} =====\n"
        prompt += load_file(cf)
    # Call LLaMA
    resp = llm(prompt=prompt, max_tokens=2048)
    return resp["choices"][0]["text"]

def phase_generate():
    print("\n🚀 Phase 1: Generating initial tests")
    for fname in os.listdir(INPUT_DIR):
        if not fname.endswith(".cc"):
            continue
        src = os.path.join(INPUT_DIR, fname)
        out = os.path.join(TEST_DIR, f"test_{fname}")
        test_code = call_llm(PROMPTS["generate"], [src])
        save_file(out, test_code)
        print(f"  ➜ Generated {out}")

def phase_refine():
    print("\n🛠 Phase 2: Refining tests")
    for fname in os.listdir(TEST_DIR):
        if not fname.startswith("test_") or not fname.endswith(".cc"):
            continue
        test_path = os.path.join(TEST_DIR, fname)
        # find corresponding source file for context
        src_name = fname.replace("test_", "")
        src_path = os.path.join(INPUT_DIR, src_name)
        refined = call_llm(PROMPTS["refine"], [src_path, test_path])
        save_file(test_path, refined)
        print(f"  ➜ Refined {test_path}")

def phase_build_and_fix():
    print("\n🔨 Phase 3: Build & fix broken tests")
    # run the build script
    subprocess.call([BUILD_SCRIPT])
    # inspect the build log
    log_path = os.path.join(LOG_DIR, "build_errors.log")
    with open(log_path, "r") as f:
        log = f.read()
    if "error" in log.lower():
        print("  ⚠️  Build errors detected—fixing tests")
        for fname in os.listdir(TEST_DIR):
            if not fname.startswith("test_") or not fname.endswith(".cc"):
                continue
            test_path = os.path.join(TEST_DIR, fname)
            # send test + log + source to LLM
            src_name = fname.replace("test_", "")
            src_path = os.path.join(INPUT_DIR, src_name)
            fix_prompt = load_file(PROMPTS["fix"])
            combined = f"{fix_prompt}\n\n// Build Log:\n{log}"
            # call LLM
            resp = llm(prompt=combined, max_tokens=2048)
            fixed = resp["choices"][0]["text"]
            save_file(test_path, fixed)
            print(f"  ➜ Fixed {test_path}")
        # rerun build one more time
        print("  🔄 Re-running build after fixes")
        subprocess.call([BUILD_SCRIPT])
    else:
        print("✅ Build passed with no errors")

if __name__ == "__main__":
    os.makedirs(TEST_DIR, exist_ok=True)
    os.makedirs(LOG_DIR, exist_ok=True)

    phase_generate()
    phase_refine()
    phase_build_and_fix()
    print("\n🎉 All phases complete. Check `tests/`, `logs/`, and run `coverage.sh` next.")
def phase_debug_and_iterate():
    print("\n🔁 Phase 4: Debug & Iterate on failing tests")

    log_path = os.path.join(LOG_DIR, "test_output.log")
    if not os.path.exists(log_path):
        print("  ⚠️  No test output found. Run tests first.")
        return
    test_log = load_file(log_path)
    if "FAILED" not in test_log:
        print("✅ All tests passed. No debug needed.")
        return

    debug_prompt = load_file("prompts/prompt4.yaml")

    for fname in os.listdir(TEST_DIR):
        if not fname.startswith("test_") or not fname.endswith(".cc"):
            continue
        test_path = os.path.join(TEST_DIR, fname)
        src_path = os.path.join(INPUT_DIR, fname.replace("test_", ""))

        # Compose debug prompt
        full_prompt = (
            debug_prompt +
            f"\n\n// Test Output:\n{test_log}\n\n" +
            f"// ===== Source: {src_path} =====\n" + load_file(src_path) +
            f"\n\n// ===== Current Test: {test_path} =====\n" + load_file(test_path)
        )

        # Call LLM
        try:
            resp = llm(prompt=full_prompt, max_tokens=2048)
            fixed_test = resp["choices"][0]["text"]
            save_file(test_path, fixed_test)
            print(f"  🔧 Debugged {test_path}")
        except Exception as e:
            print(f"  ❌ LLM failed on {fname}: {e}")
