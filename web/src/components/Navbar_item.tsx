import React from "react";
import Link from "next/link";

export function Navbar_item({
  children,
  href,
  navbar,
  external = false
}: {
  children;
  href: string;
  navbar: boolean;
  external?: boolean;
}) {
  const element: JSX.Element = external ? (
    <a
      href={href}
      target="_blank"
      className={`navbar-item ${!navbar ? "active" : ""}`}
    >
      {children}
    </a>
  ) : (
    <Link href={href} passHref>
      <a className={`navbar-item ${!navbar ? "active" : ""}`}>{children}</a>
    </Link>
  );

  return element;
}
