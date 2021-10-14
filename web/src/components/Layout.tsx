import React, { useEffect } from "react";
import Head from "next/head";

export function Layout({ children, title, home = false }) {
  return (
    <div className="min-h-screen bg-gray-900">
      <Head>
        <title>{home ? `XClicker - ${title}` : `${title} - XClicker`}</title>
        <meta charSet="utf-8" />
        <meta name="viewport" content="initial-scale=1.0, width=device-width" />
        <link rel="icon" href="/favicon.ico" />
      </Head>

      {children}
    </div>
  );
}

export default Layout;
