export default defineNuxtConfig({
  compatibilityDate: "2024-11-01",
  devtools: { enabled: true },

  future: {
    compatibilityVersion: 4,
  },

  modules: ["@nuxt/ui", "@nuxt/scripts"],
  css: ["~/assets/css/main.css"],

  vue: {
    compilerOptions: {
      isCustomElement: (tag) => tag.startsWith("esp-web-"),
    },
  },

  scripts: {
    globals: {
      espWebTools:
        "https://unpkg.com/esp-web-tools@10/dist/web/install-button.js?module",
    },
  },
});
