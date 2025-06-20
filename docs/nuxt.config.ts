export default defineNuxtConfig({
  compatibilityDate: "2024-11-01",
  devtools: { enabled: true },

  future: {
    compatibilityVersion: 4,
  },

  modules: ["@nuxt/ui", "@nuxt/scripts", "@nuxt/image", "@nuxtjs/robots"],
  css: ["~/assets/css/main.css"],

  vue: {
    compilerOptions: {
      isCustomElement: (tag) => tag.startsWith("esp-web-"),
    },
  },

  app: {
    baseURL: "/wordclock/",
    head: {
      // Meta tags to prevent github from caching the page for ages
      meta: [
        { "http-equiv": "cache-control", content: "no-cache" },
        { "http-equiv": "expires", content: "0" },
        { "http-equiv": "pragma", content: "no-cache" },
      ],
    },
  },

  scripts: {
    globals: {
      espWebTools:
        "https://unpkg.com/esp-web-tools@10/dist/web/install-button.js?module",
    },
  },

  image: {
    dir: "assets/img",
  },

  robots: { robotsTxt: false },
  site: { indexable: false },
});
