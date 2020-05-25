module.exports = {
  plugins: [
    "tailwindcss",
    "postcss-flexbugs-fixes",
    [
      "postcss-preset-env",
      {
        autoprefixer: {
          flexbox: "no-2009",
        },
        stage: 3,
        features: {
          "custom-properties": false,
        },
      },
    ],
    process.env.NODE_ENV === "production"
      ? [
          "@fullhuman/postcss-purgecss",
          {
            content: ["./src/**/*.js"],
            whitelist: [
              "awsappsync",
              "awsappsync--rehydrating",
              "app",
              "__next",
              "mt-1/2",
              "ml-1/2",
              "button"
            ],
          },
        ]
      : null,
  ].filter(Boolean),
};
