import { defineConfig } from 'vitest/config'

export default defineConfig({
  test: {
    include: ['src/web/**/*.{test,spec}.?(c|m)[jt]s?(x)']
  },
})
