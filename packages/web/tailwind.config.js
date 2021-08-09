module.exports = {
  important: true,
  purge: {
    content: ["./src/**/*.js"],
  },
  theme: {
    fontFamily: {
      sans: [
        "Roboto Mono",
        "system-ui",
        "-apple-system",
        "BlinkMacSystemFont",
        "Segoe\\ UI",
        "Roboto",
        "Helvetica\\ Neue",
        "Arial",
        "Noto\\ Sans",
        "sans-serif",
        "Apple Color\\ Emoji",
        "Segoe\\ UI\\ Emoji",
        "Segoe\\ UI\\ Symbol",
        "Noto\\ Color\\ Emoji",
      ],
      serif: [
        "Prata",
        "Georgia",
        "Cambria",
        "Times\\ New\\ Roman",
        "Times",
        "serif",
      ],
    },
    extend: {
      spacing: {
        "14": "3.5rem",
      },
      margin: {
        "1/2": "0.2rem",
      },
      borderWidth: {
        "1/2": "0.5px",
      },
      listStyleType: {
        square: "square",
      },
      backgroundColor: {
        black: '#0C0C0C',
        // white
        lightPrimary: "rgba(255, 255, 255, .92)",
        lightSecondary: "rgba(255, 255, 255, .72)",
        lightDisabled: "rgba(255, 255, 255, .50)",
        lightDivider: "rgba(255, 255, 255, .12)",

        // accent black
        darkAccent: "#2A2A2A",
        lightAccent: "#E0E0E0",

        // used for fix width backgrounds
        backgroundGray: "rgb(229, 229, 229)",

        errorRed: "#B00020",
      },
      borderColor: {
        lightPrimary: "rgba(255, 255, 255, .92)",
        lightSecondary: "rgba(255, 255, 255, .72)",
        lightDisabled: "rgba(255, 255, 255, .50)",
        lightDivider: "rgba(255, 255, 255, .12)",
      },
      textColor: {
        lightPrimary: "rgba(255, 255, 255, .92)",
        lightSecondary: "rgba(255, 255, 255, .72)",
        lightDisabled: "rgba(255, 255, 255, .50)",
        darkPrimary: "rgba(0, 0, 0, .92)",
        darkSecondary: "rgba(0, 0, 0, .72)",
        darkDisabled: "rgba(0, 0, 0, .50)"
      },
      zIndex: {
        "-10": "-10",
      },
    },
  },
};
