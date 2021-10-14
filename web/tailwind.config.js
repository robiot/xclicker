const colors = require("tailwindcss/colors");

module.exports = {
  purge: ['./src/pages/**/*.{js,ts,jsx,tsx}', './src/components/**/*.{js,ts,jsx,tsx}'],
  darkMode: false, // or 'media' or 'class'
  theme: {
    extend: {
      colors: {
        "gradient-gray": "#1e3135",
        "gradient-gray-less": "#3d5062",
        "gray": colors.trueGray,
      },
    },
  },
  variants: {
    extend: {},
  },

  plugins: [
    require('tailwindcss-container-sizes')(),
  ],
};
