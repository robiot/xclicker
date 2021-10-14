import React, { useEffect, useState } from "react";
import Link from "next/link";
import { GitHub, Twitter } from "@material-ui/icons";
import { Navbar_item } from "./Navbar_item";

export default function Nav() {
  const [navbar, setNavbar] = useState(false);

  const changeNavbar = () => {
    setNavbar(!(window.scrollY <= 10));
    console.log(navbar);
  };
  useEffect(() => {
    window.addEventListener("scroll", changeNavbar);
  });
  return (
    <div className={`sticky top-0 z-50 navbar ${navbar ? "active" : ""}`}>
      <div className="text-white flex items-center justify-between container-xl mx-auto px-4 h-16">
        {/* Left */}
        <div className="flex h-full">
          <Link href="/">
            <a className={`navbar-item ${!navbar ? "active" : ""}`}>
              <img
                src="https://raw.githubusercontent.com/robiot/XClicker/main/img/icon.png"
                className="h-12 mr-2"
              />
              <div className="text-xl font-semibold">XClicker</div>
            </a>
          </Link>

					<div className="hidden md:flex">
						<Navbar_item href="/downloads" navbar={navbar}>
							Downloads
						</Navbar_item>

						<Navbar_item href="/screenshots" navbar={navbar}>
							Screenshots
						</Navbar_item>
					</div>
        </div>

        <div className="flex h-full">
          <Navbar_item
            href="https://twitter.com/realrobiot"
            navbar={navbar}
            external={true}
          >
            <Twitter />
          </Navbar_item>

          <Navbar_item
            href="https://github.com/robiot/XClicker"
            navbar={navbar}
            external={true}
          >
            <GitHub />
          </Navbar_item>
        </div>
      </div>
    </div>
  );
}
