import { useEffect, useRef, useState } from "react";
import Layout from "../components/Layout";
import Nav from "../components/Nav";

export default function Test() {
  useEffect(() => {
    var started,
      resetTimeoutHandle,
      resetTimeout = 1000,
			counter = document.getElementById('counter'),
      zone = document.getElementById("click_button"),
      clicks = 0;

    function clicksPerSecond(started: number, clicks: number) {
      const number = (clicks / (new Date().valueOf() - started)) * 1000;
			if (number == Infinity)
				return 0;
			return number;
		}

    function count() {
      clearTimeout(resetTimeoutHandle);
      clicks++;
      counter.innerText = clicksPerSecond(started, clicks).toString();
      resetTimeoutHandle = setTimeout(reset, resetTimeout);
      return false;
    }

    function start() {
      started = new Date();
      clicks = 0;
      counter.style.opacity = "1";
      this.onmousedown = count;
      this.onmousedown();
      return false;
    }

    function reset() {
      zone.onmousedown = start;
      counter.style.opacity = "0.3";
    }

    reset();
  });

  return (
    <Layout title="Test it">
      <div className="bg-gradient-gray h-screen">
        <Nav />

        <div className="container-xl mx-auto px-10 mt-10">
          <div className="font-semibold text-3xl">Test it</div>
          <div>Here you can test XClicker or just test your cps normally</div>

          <div className="mt-10 p-5 bg-gradient-gray-less rounded-md">
            <div
              id="click_button"
              className="font-semibold text-xl mb-2 bg-gray-600 bg-opacity-20 hover:bg-opacity-30 shadow-lg rounded-md w-full h-40 flex items-center justify-center cursor-pointer"
            >
              Click me
            </div>
            <div className="font-semibold text-xl">
              Clicks per second: <span id="counter">0</span>
            </div>
          </div>
        </div>
      </div>
    </Layout>
  );
}
