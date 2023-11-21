// @ts-check
// Note: type annotations allow type checking and IDEs autocompletion

import path from "path";

require("dotenv").config();

const codeTheme = require("prism-react-renderer").themes.dracula;

/** @type {import('@docusaurus/types').Config} */
const config = {
  title: "Nitro",
  tagline: "Fast inference engine",
  favicon: "img/favicon.ico",

  // Set the production url of your site here
  url: "https://nitro.jan.ai",
  // Set the /<baseUrl>/ pathname under which your site is served
  // For GitHub pages deployment, it is often '/<projectName>/'
  baseUrl: "/",

  // GitHub pages deployment config.
  // If you aren't using GitHub pages, you don't need these.
  organizationName: "janhq", // Usually your GitHub org/user name.
  projectName: "nitro", // Usually your repo name.

  onBrokenLinks: "warn",
  onBrokenMarkdownLinks: "warn",

  // Even if you don't use internalization, you can use this field to set useful
  // metadata like html lang. For example, if your site is Chinese, you may want
  // to replace "en" with "zh-Hans".
  i18n: {
    defaultLocale: "en",
    locales: ["en"],
  },
  
  markdown: {
    mermaid: true,
  },

  // Plugins we added
  plugins: [
    "docusaurus-plugin-sass",
    async function myPlugin(context, options) {
      return {
        name: "docusaurus-tailwindcss",
        configurePostCss(postcssOptions) {
          // Appends TailwindCSS and AutoPrefixer.
          postcssOptions.plugins.push(require("tailwindcss"));
          postcssOptions.plugins.push(require("autoprefixer"));
          return postcssOptions;
        },
      };
    },
    [
      "posthog-docusaurus",
      {
        apiKey: process.env.POSTHOG_PROJECT_API_KEY || "XXX",
        appUrl: process.env.POSTHOG_APP_URL || "XXX", // optional
        enableInDevelopment: false, // optional
      },
    ],
  ],

  // The classic preset will relay each option entry to the respective sub plugin/theme.
  presets: [
    [
      "classic",
      /** @type {import('@docusaurus/preset-classic').Options} */
      ({
        // Will be passed to @docusaurus/plugin-content-docs (false to disable)
        docs: {
          routeBasePath: "/",
          sidebarPath: "./sidebars.js",
          editUrl: "https://github.com/janhq/nitro/tree/main/docs",
          showLastUpdateAuthor: true,
          showLastUpdateTime: true,
        },
        // Will be passed to @docusaurus/plugin-content-sitemap (false to disable)
        sitemap: {
          changefreq: "daily",
          priority: 1.0,
          ignorePatterns: ["/tags/**"],
          filename: "sitemap.xml",
        },
        // Will be passed to @docusaurus/plugin-content-blog (false to disable)
        blog: {
          blogSidebarTitle: "All Posts",
          blogSidebarCount: "ALL",
        },
        // Will be passed to @docusaurus/theme-classic.
        theme: {
          customCss: "./src/styles/main.scss",
        },
        // Will be passed to @docusaurus/plugin-content-pages (false to disable)
        // pages: {},
      }),
    ],
    // Redoc preset
    [
      "redocusaurus",
      {
        config: path.join(__dirname, "redocly.yaml"),
        specs: [
          {
            spec: "openapi/NitroAPI.yaml", // can be local file, url, or parsed json object
            // spec: "openapi/OpenAIAPI.yaml",
            route: "/api-reference/",
          },
        ],
        theme: {
          primaryColor: "#1a73e8",
          primaryColorDark: "#1a73e8",
          // redocOptions: { hideDownloadButton: false },
        },
      },
    ],
  ],

  // Docs: https://docusaurus.io/docs/api/themes/configuration
  themeConfig:
    /** @type {import('@docusaurus/preset-classic').ThemeConfig} */
    ({
      image: "img/nitro-social-card.png",
      // Only for react live
      liveCodeBlock: {
        playgroundPosition: "bottom",
      },
      navbar: {
        title: "Nitro",
        logo: {
          alt: "Nitro Logo",
          src: "img/logos/nitro.svg",
        },
        items: [
          // Navbar left
          {
            type: "docSidebar",
            sidebarId: "docsSidebar",
            position: "left",
            label: "Documentation",
          },
          {
            type: "docSidebar",
            sidebarId: "apiSidebar",
            position: "left",
            label: "API Reference",
          },
          // {
          //   type: "docSidebar",
          //   sidebarId: "communitySidebar",
          //   position: "left",
          //   label: "Community",
          // },
          // Navbar right
          // {
          //   type: "docSidebar",
          //   sidebarId: "blogSidebar",
          //   position: "right",
          //   label: "Blog",
          // },
        ],
      },
      prism: {
        theme: codeTheme,
        darkTheme: codeTheme,
        additionalLanguages: ["python", "powershell", "bash"],
      },
      colorMode: {
        defaultMode: "dark",
        disableSwitch: false,
        respectPrefersColorScheme: false,
      },
    }),
    // Only for react live
    themes: ["@docusaurus/theme-live-codeblock", "@docusaurus/theme-mermaid"],
};

module.exports = config;
