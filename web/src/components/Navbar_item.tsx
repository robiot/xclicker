import React from "react";
import { Link } from "react-router-dom";

export function Navbar_item({
  children,
  href,
  navbar,
  external = false,
}: {
  children: any;
  href: string;
  navbar: boolean;
  external?: boolean;
}) {
  const element: JSX.Element = external ? (
    <a
      href={href}
      target="_blank"
      rel="noopener noreferrer"
      className={`navbar-item ${!navbar ? "active" : ""}`}
    >
      {children}
    </a>
  ) : (
    <Link to={href} className={`navbar-item ${!navbar ? "active" : ""}`}>
      {children}
    </Link>
  );

  return element;
}
